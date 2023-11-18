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

int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  errif(socket_fd == -1, "socket create error");

  struct sockaddr_in serv_addr {};
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(9800);

  errif(bind(socket_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1,
        "socket bind error");
  errif(listen(socket_fd, SOMAXCONN) == -1, "socket listen error");

  int epoll_fd = epoll_create1(0);
  errif(epoll_fd == -1, "epoll create error");

  struct epoll_event events[MAX_EVENTS], ev;
  bzero(&events, sizeof(events));
  bzero(&ev, sizeof(ev));
  ev.data.fd = socket_fd;
  ev.events = EPOLLIN | EPOLLET;
  setnonblocking(socket_fd);
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev);

  while (true) {
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    errif(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i) {
      if (events[i].data.fd == socket_fd) {
        struct sockaddr_in client_addr;
        bzero(&client_addr, sizeof(client_addr));
        socklen_t client_addr_len = sizeof(client_addr);

        int client_socket_fd =
            accept(socket_fd, (sockaddr *)&client_addr, &client_addr_len);
        errif(client_socket_fd == -1, "socket accept error");
        printf("new client fd %d! IP: %s Port: %d\n", client_socket_fd,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        bzero(&ev, sizeof(ev));
        ev.data.fd = client_socket_fd;
        ev.events = EPOLLIN | EPOLLET;
        setnonblocking(client_socket_fd);
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket_fd, &ev);
      } else if (events[i].events && EPOLLIN) {
        char buf[READ_BUFFER];
        while (true) {
          bzero(&buf, sizeof(buf));
          ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
          if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", events[i].data.fd, buf);
            write(events[i].data.fd, buf, sizeof(buf));
          } else if (bytes_read == -1 && errno == EINTR) {
            printf("continue reading");
            continue;
          } else if (bytes_read == -1 &&
                     ((errno == EAGAIN) || (errno == EWOULDBLOCK))) {
            printf("client fd %d disconnected\n", events[i].data.fd);
            break;
          } else if (bytes_read == 0) {
            printf("EOF, client fd %d disconnected\n", events[i].data.fd);
            close(events[i].data.fd);
            break;
          }
        }
      } else {
        printf("something else happened\n");
      }
    }
  }
  close(socket_fd);
  return 0;
}
