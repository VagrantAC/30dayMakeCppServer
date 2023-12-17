#pragma once

#include "Channel.h"
#include "Epoll.h"

class Epoll;
class Channel;
class EventLoop {
private:
  Epoll *ep;
  bool quit;

public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel *);
};