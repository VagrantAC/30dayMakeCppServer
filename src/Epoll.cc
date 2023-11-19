#include "Epoll.h"
#include "util.h"
#include <string.h>
#include <unistd.h>

#define MAX_EVENTS 1000

Epoll::Epoll() : fd(-1), events(nullptr) {
  fd = epoll_create1(0);
  errif(fd == -1, "epoll create error");
  events = new epoll_event[MAX_EVENTS];
  bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
  delete[] events;
}

void Epoll::addFd(int socket_fd, uint32_t op) {
  struct epoll_event event;
  bzero(&event, sizeof(event));
  event.data.fd = socket_fd;
  event.events = op;
  errif(epoll_ctl(fd, EPOLL_CTL_ADD, socket_fd, &event) == -1,
        "epoll add event error");
}

std::vector<epoll_event> Epoll::poll(int timeout) {
  std::vector<epoll_event> activeEvents;
  int nfds = epoll_wait(fd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; ++i) {
    activeEvents.push_back(events[i]);
  }
  return activeEvents;
}