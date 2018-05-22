#include "timer.h"

void Timer::Restart(Timestamp now) {
    if (repeat_) {
        expiration_ = Timestamp::AddTime(now, interval_);
    } else {
        expiration_ = Timestamp();
    }
}
