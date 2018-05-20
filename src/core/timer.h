#ifndef SABER_TIMER_H
#define SABER_TIMER_H

#include <functional>
#include "timer_stamp.h"
#include "atomic.h"

using namespace saber;

class Timer {
public:
    typedef std::function<void()> TimerCallback;

    Timer(const TimerCallback &cb, Timestamp when, double interval)
    : callback_(cb),
      expiration_(when),
      interval_(interval),
      repeat_(interval > 0.0),
      sequence_(s_numCreated_.incrementaAndGet())
    {}

    void Run() const { callback_(); }
    void Restart(Timestamp now);
    static int64_t NumCreated() { return s_numCreated_.get(); }

    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }


private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;

    static AtomicInt64 s_numCreated_;
};


#endif //SABER_TIMER_H
