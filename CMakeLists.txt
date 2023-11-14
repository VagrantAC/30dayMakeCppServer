cmake_minimum_required(VERSION 3.16)
project(30dayMakeCppServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cc)
add_executable(client src/client.cc)