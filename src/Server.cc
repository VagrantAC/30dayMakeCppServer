#include "include/Server.h"

#include <functional>
#include "include/Acceptor.h"
#include "include/Channel.h"
#include "include/Connection.h"
#include "include/EventLoop.h"
#include "include/Exception.h"
#include "include/Socket.h"
#include "include/ThreadPool.h"
#include "include/util.h"

Server::Server(EventLoop *_loop) : main_reactor_(_loop), acceptor_(nullptr), thread_poll_(nullptr) {
  if (main_reactor_ == nullptr) {
    throw Exception(ExceptionType::INVALID, "main reactor can't be nullptr!");
  }
  acceptor_ = new Acceptor(main_reactor_);
  std::function<void(Socket *)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
  acceptor_->SetNewConnectionCallback(cb);

  int size = static_cast<int>(std::thread::hardware_concurrency());
  thread_poll_ = new ThreadPool(size);
  for (int i = 0; i < size; ++i) {
    sub_reactors_.push_back(new EventLoop());
  }

  for (int i = 0; i < size; ++i) {
    std::function<void()> sub_loop = std::bind(&EventLoop::Loop, sub_reactors_[i]);
    thread_poll_->Add(std::move(sub_loop));
  }
}

Server::~Server() {
  for (EventLoop *each : sub_reactors_) {
    delete each;
  }
  delete acceptor_;
  delete thread_poll_;
}

void Server::NewConnection(Socket *sock) {
  if (sock->GetFd() == -1) {
    throw Exception(ExceptionType::INVALID_SOCKET, "New Connection error, invalid client socket!");
  }
  uint64_t random = sock->GetFd() % sub_reactors_.size();
  Connection *conn = new Connection(sub_reactors_[random], sock);
  std::function<void(Socket *)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
  conn->SetDeleteConnectionCallback(cb);
  // conn->SetOnConnectCallback(on_connect_callback_);
  conn->SetOnMessageCallback(on_message_callback_);
  connections_[sock->GetFd()] = conn;
  if (new_connect_callback_) {
    new_connect_callback_(conn);
  }
}

void Server::DeleteConnection(Socket *sock) {
  int sockfd = sock->GetFd();
  auto it = connections_.find(sockfd);
  if (it != connections_.end()) {
    Connection *conn = connections_[sockfd];
    connections_.erase(sockfd);
    delete conn;
    conn = nullptr;
  }
}

void Server::OnConnect(std::function<void(Connection *)> fn) { on_connect_callback_ = std::move(fn); }

void Server::OnMessage(std::function<void(Connection *)> fn) { on_message_callback_ = std::move(fn); }

void Server::NewConnect(std::function<void(Connection *)> fn) { new_connect_callback_ = std::move(fn); }