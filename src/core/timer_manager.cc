#include "timer_manager.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <memory.h>
#include "event_loop.h"

TimerManager::TimerManager(EventLoop *loop)
  : loop_(loop),
    timerfd_(CreateTimerfd()),
    timerfd_channel_(loop, timerfd_),
    timers_(),
    calling_expired_timers_(false)
{
    timerfd_channel_.set_readCallback(std::bind(&TimerManager::HandleRead, this));
    timerfd_channel_.EnableReading();
}

TimerManager::~TimerManager() {
    timerfd_channel_.DisableAll();
    timerfd_channel_.Remove();
    ::close(timerfd_);
    for (auto it = timers_.begin();
            it != timers_.end(); ++it) {
        delete it->second;
    }
}


std::vector<TimerManager::Entry> TimerManager::GetExpired(Timestamp now) {
    std::vector<Entry> expired;
    Entry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator it = timers_.lower_bound(sentry);
    std::copy(timers_.begin(), it, back_inserter(expired));
    timers_.erase(timers_.begin(), it);

    return expired;
}


TimerId TimerManager::AddTimer(const TimerCallback &cb,
                             Timestamp when, double interval) {
    Timer *timer = new Timer(cb, when, interval);

    bool earlierst_changed = Insert(timer);
    if (earlierst_changed) {
        ResetTimerfd(timerfd_, timer->expiration());
    }
    return TimerId(timer, timer->sequence());
}

void TimerManager::CancelTimer(TimerId timerId)
{
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    auto it = active_timers_.find(timer);
    if (it != active_timers_.end()) {
        size_t n = timers_.erase(Entry(it->first->expiration(), it->first));
        delete it->first;
        active_timers_.erase(it);
    } else if (calling_expired_timers_) {
        canceling_timers_.insert(timer);
    }
}

void TimerManager::HandleRead()
{
    Timestamp now(Timestamp::Now());
    ReadTimerfd(timerfd_, now);
    std::vector<Entry> expired = GetExpired(now);
    calling_expired_timers_ = true;
    canceling_timers_.clear();
    for (auto it = expired.begin(); it != expired.end(); ++it) {
        it->second->Run();
    }
    calling_expired_timers_ = false;
    Reset(expired, now);
}


void TimerManager::Reset(const std::vector<Entry> &expired, Timestamp now)
{
    Timestamp next_expire;
    for (auto it = expired.begin(); it != expired.end(); ++it) 
    {
        ActiveTimer timer(it->second, it->second->sequence());
        if (it->second->repeat() 
            && canceling_timers_.find(timer) == canceling_timers_.end())
        {        
            it->second->Restart(now);
            Insert(it->second);
        } else {
            delete it->second;
        } 
    }

    if (!timers_.empty()) 
    {
        next_expire = timers_.begin()->second->expiration();
    }
    if (next_expire.Valid()) 
    {
        ResetTimerfd(timerfd_, next_expire);
    }
}

bool TimerManager::Insert(Timer *timer) {
    bool earliest_changed = false;
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if (it == timers_.end() || when < it->first) {
       earliest_changed = true;
    }
    timers_.insert(Entry(when, timer));
    active_timers_.insert(ActiveTimer(timer, timer->sequence()));
    return earliest_changed;
}

int TimerManager::CreateTimerfd() {
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0) {
        printf("fail in timerfd_create.\n");
    }
    return timerfd;
}

struct timespec TimerManager::HowMuchTimeFromNow(Timestamp when) {
    int64_t microseconds = when.micro_seconds() -
                           Timestamp::Now().micro_seconds();
    if (microseconds < 100) {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void TimerManager::ReadTimerfd(int timerfd, Timestamp now)
{
    uint64_t timelong;
    ::read(timerfd, &timelong, sizeof timelong); 
}

void TimerManager::ResetTimerfd(int timerfd, Timestamp expiration) {
    struct  itimerspec newValue;
    struct  itimerspec oldValue;
    bzero(&oldValue, sizeof oldValue);
    bzero(&newValue, sizeof newValue);

    newValue.it_value = HowMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret) {
        printf("error:resetTimerfd().\n");
    }
}