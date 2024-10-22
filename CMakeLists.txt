# CMake requirements
cmake_minimum_required(VERSION 3.0)

aux_source_directory(${CMAKE_SOURCE_DIR}/include/ozz/net/src SRC_LIST)
aux_source_directory(${CMAKE_SOURCE_DIR}/include/ozz/utils/src SRC_LIST)

# add_executable(server 
# ${SRC_LIST}
#  server.cpp)

add_executable(client 
${SRC_LIST}
 client.cpp)
