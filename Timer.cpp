//
// Created by xcv on 17-12-20.
//

#include "Timer.h"

using namespace saber;

//静态变量初始化
AtomicInt64 Timer::s_numCreated_;

void Timer::restart(Timestamp now) {
    if (repeat_) {
        expiration_ = Timestamp::addTime(now, interval_);
    } else {
        expiration_ = Timestamp();
    }
}
