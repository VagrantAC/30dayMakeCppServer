#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "util.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

void handleReadEvent(int);

int main() {
  Socket *serv_sock = new Socket();
  InetAddress *serv_addr = new InetAddress("127.0.0.1", 9800);
  serv_sock->bind(serv_addr);
  serv_sock->listen();

  Epoll *ep = new Epoll();
  serv_sock->setnonblocking();
  ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);

  while (true) {
    std::vector<epoll_event> events = ep->poll();
    int nfds = events.size();
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == serv_sock->getFd()) {
        // TODO: 会发生内存泄漏
        InetAddress *client_addr = new InetAddress();
        Socket *client_socket = new Socket(serv_sock->accept(client_addr));

        printf("new client fd %d! IP: %s Port: %d\n", client_socket->getFd(),
               inet_ntoa(client_addr->addr.sin_addr),
               ntohs(client_addr->addr.sin_port));
        client_socket->setnonblocking();
        ep->addFd(client_socket->getFd(), EPOLLIN | EPOLLET);
      } else if (events[i].events && EPOLLIN) {
        handleReadEvent(events[i].data.fd);
      } else {
        printf("something else happened\n");
      }
    }
  }
  delete serv_sock;
  delete serv_addr;
  return 0;
}

void handleReadEvent(int socket_fd) {
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