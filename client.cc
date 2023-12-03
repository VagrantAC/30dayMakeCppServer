#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/util.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  Socket *cli_sock1 = new Socket();
  Socket *cli_sock2 = new Socket();
  InetAddress *cli_addr = new InetAddress("127.0.0.1", 9801);
  cli_sock1->connect(cli_addr);
  cli_sock2->connect(cli_addr);

  while (true) {
    int socket_fd1 = cli_sock1->getFd();
    int socket_fd2 = cli_sock2->getFd();
    char buf[1024];
    bzero(&buf, sizeof(buf));
    scanf("%s", buf);
    ssize_t write_bytes1 = write(socket_fd1, buf, sizeof(buf));
    if (write_bytes1 == -1) {
      printf("socket1 already disconnected, can't write any more!\n");
      break;
    }
    ssize_t write_bytes2 = write(socket_fd2, buf, sizeof(buf));
    if (write_bytes2 == -1) {
      printf("socket2 already disconnected, can't write any more!\n");
      break;
    }
    bzero(&buf, sizeof(buf));
    ssize_t read_bytes1 = read(socket_fd1, buf, sizeof(buf));
    if (read_bytes1 > 0) {
      printf("message from server: %s\n", buf);
    } else if (read_bytes1 == 0) {
      printf("server socket disconnected!\n");
      break;
    } else if (read_bytes1 == -1) {
      close(socket_fd1);
      errif(true, "socket1 read error");
    }

    ssize_t read_bytes2 = read(socket_fd2, buf, sizeof(buf));
    if (read_bytes2 > 0) {
      printf("message from server: %s\n", buf);
    } else if (read_bytes2 == 0) {
      printf("server socket disconnected!\n");
      break;
    } else if (read_bytes2 == -1) {
      close(socket_fd2);
      errif(true, "socket2 read error");
    }
  }
  delete cli_sock1;
  delete cli_sock2;
  delete cli_addr;
  return 0;
}