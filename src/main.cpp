#include <iostream>
#include <sys/timefd.h>
#include "EventLoop.h"
#include "Channel.h"

EventLoop *g_loop;

void timeout() {
    std::cout << "Time out.\n" << std::endl;
    g_loop->quit();
}

int main () {
    EventLoop loop;
    g_loop = &loop;
    int timerfd = ::timefd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    Channel channel(&loop, timefd);
    channel.setReadCallback(timeout);
    Channel.enableReading();

    struct itimerspec howlong;
    bzero(&howlong, sizeof howlong);
    howlong.it_value.tv_sec = 5;
    ::timefd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();
    return 0;
}