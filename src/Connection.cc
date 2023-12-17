#include "include/Connection.h"

#include <unistd.h>
#include <cstring>
#include "include/Buffer.h"
#include "include/Channel.h"
#include "include/Socket.h"
#include "include/util.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop *loop, Socket *sock)
    : loop_(loop), sock_(sock), channel_(nullptr), read_buffer_(nullptr) {
  channel_ = new Channel(loop_, sock_->GetFd());
  channel_->EnableRead();
  channel_->UseET();
  std::function<void()> cb = std::bind(&Connection::Echo, this, sock_->GetFd());
  channel_->SetReadCallback(cb);
  read_buffer_ = new Buffer();
}

Connection::~Connection() {
  delete channel_;
  delete sock_;
  delete read_buffer_;
}

void Connection::SetDeleteConnectionCallback(std::function<void(int)> const &callback) {
  delete_connection_callback_ = callback;
}

void Connection::Echo(int sockfd) {
  char buf[READ_BUFFER];
  while (true) {
    memset(&buf, 0, sizeof(buf));
    ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buffer_->Append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
      printf("message from client fd %d: %s\n", sockfd, read_buffer_->ToStr());
      Send(sockfd);
      read_buffer_->Clear();
      break;
    } else if (bytes_read == 0) {
      printf("EOF, client fd %d disconnected\n", sockfd);
      delete_connection_callback_(sockfd);
      break;
    } else {
      printf("Connection reset by peer\n");
      delete_connection_callback_(sockfd);
      break;
    }
  }
}

void Connection::Send(int sockfd) {
  char buf[read_buffer_->Size()];
  strcpy(buf, read_buffer_->ToStr());
  int data_size = read_buffer_->Size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = write(sockfd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    data_left -= bytes_write;
  }
}