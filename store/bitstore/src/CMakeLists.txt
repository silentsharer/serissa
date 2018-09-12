# CMake minimum version
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fPIC")
add_definitions("-Wall -g")

add_subdirectory(core)
add_subdirectory(asyncio)

link_directories(core asyncio)

set(_PROTOBUF_LIBPROTOBUF libprotobuf)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protoc>)
set(_GRPC_GRPCPP_UNSECURE grpc++_unsecure)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)

# Proto file
get_filename_component(proxy_proto "./pb/bitstore.proto" ABSOLUTE)
get_filename_component(proxy_proto_path "${proxy_proto}" PATH)

# Generated sources
set(proxy_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/bitstore.pb.cc")
set(proxy_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bitstore.pb.h")
set(proxy_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/bitstore.grpc.pb.cc")
set(proxy_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/bitstore.grpc.pb.h")
add_custom_command(
        OUTPUT "${proxy_proto_srcs}" "${proxy_proto_hdrs}" "${proxy_grpc_srcs}" "${proxy_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${proxy_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${proxy_proto}"
        DEPENDS "${proxy_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Building server
add_executable(bitstore
        main.cc
        server/bitstore.cc
        ${proxy_proto_srcs}
        ${proxy_grpc_srcs}
        )

# Link lib
target_link_libraries(bitstore
        core
        asyncio
        ${_GRPC_GRPCPP_UNSECURE}
        ${_PROTOBUF_LIBPROTOBUF}
        aio
        )
