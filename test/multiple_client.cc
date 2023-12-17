#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include "Buffer.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "util.h"

void OneClient(int msgs, int wait) {
  Socket *socket = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 9801);
  socket->Connect(addr);

  int socketfd = socket->GetFd();

  Buffer *sendBuffer = new Buffer();
  Buffer *readBuffer = new Buffer();

  sleep(wait);
  int count = 0;
  while (count < msgs) {
    sendBuffer->SetBuf("I'm client!");
    ssize_t write_bytes = write(socketfd, sendBuffer->ToStr(), sendBuffer->Size());
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    int already_read = 0;
    char buf[1024];
    while (true) {
      bzero(&buf, sizeof(buf));
      ssize_t read_bytes = read(socketfd, buf, sizeof(buf));
      if (read_bytes > 0) {
        readBuffer->Append(buf, read_bytes);
        already_read += read_bytes;
      } else if (read_bytes == 0) {
        printf("server disconnected!\n");
        exit(EXIT_SUCCESS);
      }
      if (already_read >= sendBuffer->Size()) {
        printf("count: %d, message from server: %s\n", count++, readBuffer->ToStr());
        break;
      }
    }
    readBuffer->Clear();
  }
  delete addr;
  delete socket;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = std::stoi(optarg);
        break;
      case 'm':
        msgs = std::stoi(optarg);
        break;
      case 'w':
        wait = std::stoi(optarg);
        break;
      case '?':
        printf("error optopt: %c\n", optopt);
        printf("error opterr: %d\n", opterr);
        break;
    }
  }

  ThreadPool *poll = new ThreadPool(threads);
  std::function<void()> func = std::bind(OneClient, msgs, wait);
  for (int i = 0; i < threads; ++i) {
    poll->Add(func);
  }
  delete poll;
  return 0;
}