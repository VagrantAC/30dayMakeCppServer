#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/util.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  Socket *cli_sock = new Socket();
  InetAddress *cli_addr = new InetAddress("127.0.0.1", 9800);
  cli_sock->connect(cli_addr);

  while (true) {
    int socket_fd = cli_sock->getFd();
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
  delete cli_sock;
  delete cli_addr;
  return 0;
}