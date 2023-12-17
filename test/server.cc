#include "Server.h"
#include "EventLoop.h"

int main() {
  EventLoop *loop = new EventLoop();
  new Server(loop);
  loop->Loop();
  return 0;
}