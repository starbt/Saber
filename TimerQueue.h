//
// Created by xcv on 17-12-20.
//

#ifndef SABER_TIMERQUEUE_H
#define SABER_TIMERQUEUE_H


#include <set>
#include "EventLoop.h"
#include "base/TimerId.h"
#include "base/Timestamp.h"
#include "Timer.h"

class TimerQueue {
public:
    typedef std::function<void()> TimerCallback;
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback &cb,
                      Timestamp when,
                       double interval);
    void addTimerInLoop(Timer *timer);

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry> &expired, Timestamp now);

    bool insert(Timer *timer);

    EventLoop *loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    TimerList timers_;

    //for cancel()
    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_;   /*atomic*/
    ActiveTimerSet cancelingTimers_;
};


#endif //SABER_TIMERQUEUE_H
