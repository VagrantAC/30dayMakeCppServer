cmake_minimum_required(VERSION 3.16)
project(30dayMakeCppServer)

set(CMAKE_CXX_STANDARD 17)

set(SHARED_SOURCES
    src/Epoll.h
    src/Epoll.cc
    src/InetAddress.h
    src/InetAddress.cc
    src/Socket.h
    src/Socket.cc
    src/util.h
    src/util.cc
    src/Channel.h
    src/Channel.cc
)

add_executable(server 
    src/server.cc
    ${SHARED_SOURCES}
)

add_executable(client 
    src/client.cc
    ${SHARED_SOURCES}
)