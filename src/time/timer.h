#ifndef SABER_TIMER_H
#define SABER_TIMER_H

#include <functional>
#include "timer_stamp.h"

class Timer {
public:
    typedef std::function<void()> TimerCallback;

    Timer(const TimerCallback &cb, Timestamp when, double interval)
    : callback_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0)
    {}

    void Run() const { callback_(); }
    void Restart(Timestamp now);
    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    void set_circle_num(int circle_num) { circle_num_ = circle_num; }
    int circle_num() { return circle_num_; }


private:
    const TimerCallback callback_;
    Timestamp expiration_;
    int circle_num_;
    const double interval_;
    const bool repeat_;
};


#endif //SABER_TIMER_H
