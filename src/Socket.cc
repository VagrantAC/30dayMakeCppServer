#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket() : fd(-1) {
  fd = socket(AF_INET, SOCK_STREAM, 0);
  errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd) { errif(fd == -1, "socket create error"); }

Socket::~Socket() {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}

void Socket::bind(InetAddress *addr) {
  errif(::bind(fd, (sockaddr *)&addr->addr, addr->addr_len) == -1,
        "socket bind error");
}

void Socket::listen() {
  errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking() {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void Socket::connect(InetAddress *addr) {
  errif(::connect(fd, (sockaddr *)&addr->addr, addr->addr_len) == -1,
        "socket connect error");
}

int Socket::accept(InetAddress *addr) {
  int client_socket_fd = ::accept(fd, (sockaddr *)&addr->addr, &addr->addr_len);
  errif(client_socket_fd == -1, "socket accept error");
  return client_socket_fd;
}

int Socket::getFd() { return fd; }