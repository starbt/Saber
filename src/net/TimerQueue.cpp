#include <sys/timerfd.h>
#include <unistd.h>
#include <memory.h>

#include "TimerQueue.h"
#include "EventLoop.h"

TimerQueue::TimerQueue(EventLoop *loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_(),
    callingExpiredTimers_(false)
{
    timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {
    timerfdChannel_.disableAll();
    timerfdChannel_.remove();
    ::close(timerfd_);
    for (auto it = timers_.begin();
            it != timers_.end(); ++it) {
        delete it->second;
    }
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), it, back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    return expired;
}


TimerId TimerQueue::addTimer(const TimerCallback &cb,
                             Timestamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);

    bool earlierstChanged = insert(timer);
    if (earlierstChanged) {
        resetTimerfd(timerfd_, timer->expiration());
    }
    return TimerId(timer, timer->sequence());
}

void TimerQueue::cancelTimer(TimerId timerId)
{
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer);
    }
}

void TimerQueue::handleRead()
{
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd_, now);
    std::vector<Entry> expired = getExpired(now);
    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (auto it = expired.begin(); it != expired.end(); ++it) {
        it->second->run();
    }
    callingExpiredTimers_ = false;
    reset(expired, now);
}


void TimerQueue::reset(const std::vector<Entry> &expired, Timestamp now)
{
    Timestamp nextExpire;
    for (auto it = expired.begin(); it != expired.end(); ++it) 
    {
        ActiveTimer timer(it->second, it->second->sequence());
        if (it->second->repeat() 
            && cancelingTimers_.find(timer) == cancelingTimers_.end())
        {        
            it->second->restart(now);
            insert(it->second);
        } else {
            delete it->second;
        } 
    }

    if (!timers_.empty()) 
    {
        nextExpire = timers_.begin()->second->expiration();
    }
    if (nextExpire.valid()) 
    {
        resetTimerfd(timerfd_, nextExpire);
    }
}

bool TimerQueue::insert(Timer *timer) {
    bool earliestChanged = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
       earliestChanged = true;
    }
    timers_.insert(Entry(when, timer));
    activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    return earliestChanged;
}

int TimerQueue::createTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        printf("fail in timerfd_create.\n");
    }
    return timerfd;
}

struct timespec TimerQueue::howMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.microSecondsSinceEpoch() -
                           Timestamp::now().microSecondsSinceEpoch();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void TimerQueue::readTimerfd(int timerfd, Timestamp now)
{
    uint64_t timelong;
    ::read(timerfd, &timelong, sizeof timelong); 
}

void TimerQueue::resetTimerfd(int timerfd, Timestamp expiration) {
    struct  itimerspec newValue;
    struct  itimerspec oldValue;
    bzero(&oldValue, sizeof oldValue);
    bzero(&newValue, sizeof newValue);

    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        printf("error:resetTimerfd().\n");
    }
}