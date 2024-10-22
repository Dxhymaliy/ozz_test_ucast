cmake_minimum_required(VERSION 3.16)
project(ozz_udp)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -g -std=c++2a -fno-stack-protector -Wall -pthread")
# All includes referenced here with 'stg' lead prefix
include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB_RECURSE INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include/*.hpp )

set(PROTOCOL_VERSION 1.0)
configure_file(${CMAKE_SOURCE_DIR}/version.hpp.in
            ${CMAKE_BINARY_DIR}/version/version.hpp)
include_directories(${CMAKE_BINARY_DIR}/version)


add_subdirectory(${PROJECT_SOURCE_DIR}/src)
