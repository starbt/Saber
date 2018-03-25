//
// Created by xcv on 17-12-20.
//

#ifndef SABER_TIMERID_H
#define SABER_TIMERID_H


#include <cstdint>
#include <cstddef>

class Timer;

class TimerId {
public:
    friend class TimerQueue;

    TimerId()
      : timer_(NULL),
       sequence_(0)
    { }

    TimerId(Timer *timer, int64_t seq)
      : timer_(timer),
        sequence_(seq)
    { }

private:
    Timer *timer_;
    int64_t sequence_;
};


#endif //SABER_TIMERID_H
