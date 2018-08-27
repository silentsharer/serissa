# CMake minimum version
cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -fPIC")
add_definitions("-Wall -g")

add_subdirectory(core)

# Building server
add_executable(bitstore
        bitstore.cc
)

# Link lib
target_link_libraries(bitstore
        glog
        core
)