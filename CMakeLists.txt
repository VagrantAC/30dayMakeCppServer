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
    src/EventLoop.h
    src/EventLoop.cc
    src/Server.h
    src/Server.cc
    src/Acceptor.h
    src/Acceptor.cc
    src/Connection.h
    src/Connection.cc
)

add_executable(server 
    server.cc
    ${SHARED_SOURCES}
)

add_executable(client 
    client.cc
    ${SHARED_SOURCES}
)