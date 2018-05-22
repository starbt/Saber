#ifndef SABER_TIMER_MANAGER_H
#define SABER_TIMER_MANAGER_H

#include "timer.h"
#include <vector>
#include <list>

#include "core/fd_channel.h"
#include "timer_stamp.h"
#include "timer.h"
#include <mutex>

class EventLoop;
class Timer;

class TimerManager {
public:
    typedef std::function<void()> TimerCallback;
    static const int kInitTimeWheelSize;
    static const int kInitMutexVecSize;

    TimerManager(EventLoop* loop, int size = kInitTimeWheelSize, int mutex_vec_size = kInitMutexVecSize);
    ~TimerManager();

    void AddTimer(const TimerCallback &cb,
                      Timestamp when,
                       double interval);
    void AddTimer(Timer* timer);

    void HandleRead();
    std::vector<Timer*> GetExpired(Timestamp now);
    void Reset(const std::vector<Timer*> &expired, Timestamp now);

private:
   std::vector<std::list<Timer*>>  timer_wheel_;
   //分段锁
   std::vector<std::mutex> mutex_vec_;
   int size_;
   int mutex_vec_size_;
   int spoke_index_;
   int tick_time_;
   int circle_num_;
   EventLoop *loop_;
   const int timerfd_;
   FdChannel timerfd_channel_;

private:
   void SetTimerfd(int timerfd);
   int CreateTimerfd();
   int HowMuchTimeFromNow(Timestamp when); 

};

#endif