#include "include/Acceptor.h"
#include "include/Channel.h"
#include "include/Server.h"
#include "include/Socket.h"

Acceptor::Acceptor(EventLoop *_loop) : loop_(_loop), sock_(nullptr), channel_(nullptr) {
  sock_ = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 9801);
  sock_->Bind(addr);
  sock_->Listen();
  channel_ = new Channel(loop_, sock_->GetFd());
  std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
  channel_->SetReadCallback(cb);
  channel_->EnableRead();
  delete addr;
}

Acceptor::~Acceptor() {
  delete channel_;
  delete sock_;
}

void Acceptor::AcceptConnection() {
  InetAddress *client_addr = new InetAddress();
  Socket *client_sock = new Socket(sock_->Accept(client_addr));
  printf("new client fd %d! IP: %s Port: %d\n", client_sock->GetFd(), inet_ntoa(client_addr->GetAddr().sin_addr),
         ntohs(client_addr->GetAddr().sin_port));
  client_sock->SetNonblocking();
  new_connection_callback_(client_sock);
  delete client_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket *)> const &callback) {
  new_connection_callback_ = callback;
}