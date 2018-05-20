#include "timer.h"

//静态变量初始化
AtomicInt64 Timer::s_numCreated_;

void Timer::Restart(Timestamp now) {
    if (repeat_) {
        expiration_ = Timestamp::AddTime(now, interval_);
    } else {
        expiration_ = Timestamp();
    }
}
