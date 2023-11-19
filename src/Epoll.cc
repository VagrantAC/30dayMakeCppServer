#include "Epoll.h"
#include "Channel.h"
#include "util.h"
#include <string.h>
#include <sys/epoll.h>
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

// std::vector<epoll_event> Epoll::poll(int timeout) {
//   std::vector<epoll_event> activeEvents;
//   int nfds = epoll_wait(fd, events, MAX_EVENTS, timeout);
//   errif(nfds == -1, "epoll wait error");
//   for (int i = 0; i < nfds; ++i) {
//     activeEvents.push_back(events[i]);
//   }
//   return activeEvents;
// }

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> activeChannels;
  int nfds = epoll_wait(fd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; ++i) {
    Channel *ch = (Channel *)events[i].data.ptr;
    ch->setRevents(events[i].events);
    activeChannels.push_back(ch);
  }
  return activeChannels;
}

void Epoll::updateChannel(Channel *channel) {
  int channel_fd = channel->getFd();
  struct epoll_event ev;
  bzero(&ev, sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->getInEpoll()) {
    errif(epoll_ctl(fd, EPOLL_CTL_ADD, channel_fd, &ev) == -1,
          "epoll add error");
    channel->setInEpoll();
  } else {
    errif(epoll_ctl(fd, EPOLL_CTL_MOD, channel_fd, &ev) == -1,
          "epoll modify error");
  }
}