#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
class Epoll {
private:
  int fd;
  struct epoll_event *events;

public:
  Epoll();
  ~Epoll();

  void updateChannel(Channel *);
  void deleteChannel(Channel *);

  std::vector<Channel *> poll(int timeout = -1);
};