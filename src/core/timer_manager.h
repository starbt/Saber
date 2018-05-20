#ifndef SABER_TIMERQUEUE_H
#define SABER_TIMERQUEUE_H

#include <vector>
#include <set>

#include "fd_channel.h"
#include "timer_id.h"
#include "timer_stamp.h"
#include "timer.h"

class EventLoop;

class TimerManager {
public:
    typedef std::function<void()> TimerCallback;
    typedef std::pair<Timestamp, Timer *> Entry;
    typedef std::set<Entry> TimerList;

    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

public:
    TimerManager(EventLoop *loop);
    ~TimerManager(); 
    TimerId AddTimer(const TimerCallback &cb,
                      Timestamp when,
                       double interval);
    void CancelTimer(TimerId timerId);

    void HandleRead();
    std::vector<Entry> GetExpired(Timestamp now);
    void Reset(const std::vector<Entry> &expired, Timestamp now);


private:
   int CreateTimerfd();
   void ReadTimerfd(int timefd, Timestamp now);
   struct timespec HowMuchTimeFromNow(Timestamp when); 
   void ResetTimerfd(int timerfd, Timestamp expiration); 

   bool Insert(Timer *timer);

    EventLoop *loop_;
    const int timerfd_;
    FdChannel timerfd_channel_;
    TimerList timers_;

    //for cancel()
    ActiveTimerSet active_timers_;
    bool calling_expired_timers_;   /*atomic*/
    ActiveTimerSet canceling_timers_;
};


#endif //SABER_TIMERQUEUE_H
