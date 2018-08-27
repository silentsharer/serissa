# CMake minimum version
cmake_minimum_required (VERSION 3.0)
set(CMAKE_MACOSX_RPATH 1)

# project info
project (bitstore C CXX)
set(VERSION 0.0.1)

# include header
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# generate target
add_subdirectory(src)
add_subdirectory(third_party/glog)