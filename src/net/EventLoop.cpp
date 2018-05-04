#include "EventLoop.h"
#include "SocketsOps.h"

#include <sys/eventfd.h>

// 线程中的变量，只可以是全局变量，或者静态变量，线程安全
// __thread EventLoop *t_loopInThisThread = 0;

const int kPollTimeMs = 10 * 1000;

//help function
/* int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        printf("error:createEventfd().\n");
        abort();
    }
    return evtfd;
} */

EventLoop::EventLoop()
  : looping_(false),
    quit_(false),
    poller_(std::make_shared<Poller>(this)),
    timerQueue_(new TimerQueue(this))
{ }

EventLoop::~EventLoop() 
{ }

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);

        for (auto iter = activeChannels_.begin(); iter != activeChannels_.end(); ++iter) {
            (*iter)->handleEvent();
        }
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
}

void EventLoop::updateChannel(Channel *channel) {
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback &cb)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback &cb)
{
    Timestamp time(Timestamp::addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback &cb)
{
    Timestamp time(Timestamp::addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

