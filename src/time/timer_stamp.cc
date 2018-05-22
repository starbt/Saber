#include "timer_stamp.h"
#include <sys/time.h>
#include <cstddef>

Timestamp Timestamp::AddTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.micro_seconds_ + delta);
}

Timestamp Timestamp::Now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}
