#include "include/EventLoop.h"

#include "include/Channel.h"
#include "include/Poller.h"
#include "include/ThreadPool.h"

EventLoop::EventLoop() { poller_ = new Poller(); }

EventLoop::~EventLoop() {
  Quit();
  delete poller_;
}

void EventLoop::Loop() {
  while (!quit_) {
    std::vector<Channel *> chs;
    chs = poller_->Poll();
    for (auto &ch : chs) {
      ch->HandleEvent();
    }
  }
}

void EventLoop::Quit() { quit_ = true; }

void EventLoop::UpdateChannel(Channel *ch) { poller_->UpdateChannel(ch); }
void EventLoop::DeleteChannel(Channel *ch) { poller_->DeleteChannel(ch); }