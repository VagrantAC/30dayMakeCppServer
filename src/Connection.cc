#include "Connection.h"
#include "Buffer.h"
#include "Channel.h"
#include "Socket.h"
#include "util.h"
#include <string.h>
#include <unistd.h>

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock)
    : loop(_loop), sock(_sock), channel(nullptr), inBuffer(new std::string()),
      readBuffer(nullptr) {
  channel = new Channel(loop, sock->getFd());
  std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
  channel->setCallback(cb);
  channel->enableReading();
  readBuffer = new Buffer();
}

Connection::~Connection() {
  delete channel;
  delete sock;
}

void Connection::echo(int socket_fd) {
  char buf[READ_BUFFER];
  while (true) {
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(socket_fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      readBuffer->append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      printf("finish reading once\n");
      printf("message from client fd %d: %s\n", socket_fd, readBuffer->c_str());
      errif(write(socket_fd, readBuffer->c_str(), readBuffer->size()) == -1,
            "socket write error");
      readBuffer->clear();
      break;
    } else if (bytes_read == 0) {
      printf("EOF, client fd %d disconnected\n", socket_fd);
      deleteConnectionCallback(sock);
      break;
    }
  }
}

void Connection::setDeleteConnectionCallback(
    std::function<void(Socket *)> _cb) {
  deleteConnectionCallback = _cb;
}