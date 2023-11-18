cmake_minimum_required(VERSION 3.16)
project(30dayMakeCppServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cc
        src/util.h
        src/util.cc)
add_executable(client src/client.cc
        src/util.h
        src/util.cc)