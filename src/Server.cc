#include "Server.h"
#include "Acceptor.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"
#include <functional>
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr) {
  acceptor = new Acceptor(loop);
  std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
  acceptor->setNewConnectionCallback(cb);
}

Server::~Server() {
  delete acceptor;
}

void Server::handleReadEvent(int socket_fd) {
  char buf[READ_BUFFER];
  while (true) {
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(socket_fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", socket_fd, buf);
      write(socket_fd, buf, sizeof(buf));
    } else if (bytes_read == -1 && errno == EINTR) {
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) {
      printf("EOF, client fd %d disconnected\n", socket_fd);
      close(socket_fd);
      break;
    }
  }
}

void Server::newConnection(Socket *serv_sock) {
  // TODO: 会发生内存泄漏
  InetAddress *client_addr = new InetAddress();
  Socket *client_socket = new Socket(serv_sock->accept(client_addr));
  printf("new client fd %d! IP: %s Port: %d\n", client_socket->getFd(),
         inet_ntoa(client_addr->addr.sin_addr),
         ntohs(client_addr->addr.sin_port));
  client_socket->setnonblocking();

  Channel *client_channel = new Channel(loop, client_socket->getFd());
  std::function<void()> cb =
      std::bind(&Server::handleReadEvent, this, client_socket->getFd());
  client_channel->setCallback(cb);
  client_channel->enableReading();
}