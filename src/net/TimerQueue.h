#ifndef SABER_TIMERQUEUE_H
#define SABER_TIMERQUEUE_H

#include <vector>
#include <set>
#include "Channel.h"
#include "../base/TimerId.h"
#include "../base/Timestamp.h"
#include "Timer.h"

class EventLoop;

class TimerQueue {
public:
    typedef std::function<void()> TimerCallback;
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    TimerId addTimer(const TimerCallback &cb,
                      Timestamp when,
                       double interval);
    void cancelTimer(TimerId timerId);

    void handleRead();
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry> &expired, Timestamp now);


private:
   int createTimerfd();
   void readTimerfd(int timefd, Timestamp now);
   struct timespec howMuchTimeFromNow(Timestamp when); 
   void resetTimerfd(int timerfd, Timestamp expiration); 

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
