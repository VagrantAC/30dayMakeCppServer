#include "Server.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Socket.h"
#include <functional>

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr) {
  acceptor = new Acceptor(loop);
  std::function<void(Socket *)> cb =
      std::bind(&Server::newConnection, this, std::placeholders::_1);
  acceptor->setNewConnectionCallback(cb);
}

Server::~Server() { delete acceptor; }

void Server::newConnection(Socket *sock) {
  if (sock->getFd() != -1) {
    Connection *conn = new Connection(loop, sock);
    std::function<void(int)> cb =
        std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getFd()] = conn;
  }
}

void Server::deleteConnection(int socket_fd) {
  if (socket_fd != -1) {
    auto it = connections.find(socket_fd);
    if (it != connections.end()) {
      Connection *conn = connections[socket_fd];
      connections.erase(socket_fd);
      delete conn;
    }
  }
}