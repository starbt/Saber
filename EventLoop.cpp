//
// Created by xcv on 17-12-18.
//

#include "EventLoop.h"
#include "SocketsOps.h"

#include <sys/eventfd.h>

using namespace saber;

// 线程中的变量，只可以是全局变量，或者静态变量，线程安全
__thread EventLoop *t_loopInThisThread = 0;

const int kPollTimeMs = 10 * 1000;

//help function
int createEventfd() {
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        printf("error:createEventfd().\n");
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
  : looping_(false),
    threadId_(Thread::getCurrentThreadPid()),
    quit_(false),
    callingPendingFunctors_(false),
    currentActiveChannel_(NULL),
    poller_(std::make_shared<Poller>(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd_(createEventfd()),
    wakeupChannel_(new Channel(this, wakeupFd_))
{
    if (t_loopInThisThread) {
        printf("another EventLoop is in this thread.\n");
    } else {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(
            std::bind(&EventLoop::handleRead, this)
    );
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = false;
}

void EventLoop::loop() {
    assert(!looping_);
    looping_ = true;
    quit_ = false;
    while (!quit_) {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);

        for (auto iter = activeChannels_.begin();
             iter != activeChannels_.end(); ++iter) {
            (*iter)->handleEvent();
        }
        doPendingFunctors();
    }
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::updateChannel(Channel *channel) {
    assert(channel->ownerLoop() == this);
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    poller_->removeChannel(channel);
}

void EventLoop::runInLoop(const Functor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n =  sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        printf("error::wakeup().\n");
    }
}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one) {
        printf("error:handleRead.\n");
    }
}

void EventLoop::queueInLoop(const Functor &cb) {
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }
    if (!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }

}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for (size_t i = 0; i < functors.size(); ++i) {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}
