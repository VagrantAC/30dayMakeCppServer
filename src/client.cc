#include "util.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  errif(socket_fd == -1, "socket create error");

  struct sockaddr_in serv_addr {};
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serv_addr.sin_port = htons(9800);

  errif(connect(socket_fd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1,
        "socket connect error");

  while (true) {
    char buf[1024];
    bzero(&buf, sizeof(buf));
    scanf("%s", buf);
    ssize_t write_bytes = write(socket_fd, buf, sizeof(buf));
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes = read(socket_fd, buf, sizeof(buf));
    if (read_bytes > 0) {
      printf("message from server: %s\n", buf);
    } else if (read_bytes == 0) {
      printf("server socket disconnected!\n");
      break;
    } else if (read_bytes == -1) {
      close(socket_fd);
      errif(true, "socket read error");
    }
  }
  close(socket_fd);
  return 0;
}