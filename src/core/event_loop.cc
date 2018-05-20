#include "event_loop.h"
#include <sys/eventfd.h>
#include "net/sockets_util.h"

// 线程中的变量，只可以是全局变量，或者静态变量，线程安全
// __thread EventLoop *t_loopInThisThread = 0;

const int kPollTimeMs = 10 * 1000;

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    poller_(std::make_shared<Epoller>(this)),
    timer_manager_(new TimerManager(this))
{ }

EventLoop::~EventLoop() 
{ }

void EventLoop::Loop() {
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        active_channels_.clear();
        poller_->Poll(kPollTimeMs, &active_channels_);

        for (auto iter = active_channels_.begin(); iter != active_channels_.end(); ++iter) {
            (*iter)->HandleEvent();
        }
    }
    looping_ = false;
}

void EventLoop::Quit() {
    quit_ = true;
}

void EventLoop::UpdateChannel(FdChannel *channel) {
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(FdChannel *channel) {
    poller_->RemoveChannel(channel);
}

TimerId EventLoop::RunAt(const Timestamp &time, const TimerCallback &cb)
{
    return timer_manager_->AddTimer(cb, time, 0.0);
}

TimerId EventLoop::RunAfter(double delay, const TimerCallback &cb)
{
    Timestamp time(Timestamp::AddTime(Timestamp::Now(), delay));
    return RunAt(time, cb);
}

TimerId EventLoop::RunEvery(double interval, const TimerCallback &cb)
{
    Timestamp time(Timestamp::AddTime(Timestamp::Now(), interval));
    return timer_manager_->AddTimer(cb, time, interval);
}

