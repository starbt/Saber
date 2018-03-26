#ifndef SABER_EVENTLOOP_H
#define SABER_EVENTLOOP_H

#include <vector>
#include <memory>
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"

class EventLoop {
public:
    typedef std::vector<Channel *> ChannelList;
//    typedef std::function<void ()> TimerCallback;
    EventLoop();
    ~EventLoop();

public:
    void loop();
    void quit();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    void handleRead();

    //一组定时回调函数
    TimerId runAt(const Timer &time, const TimerCallback &cb);
    TimerId runAfter(double delay, const TimerCallback &cb);
    TimerId runEvery(double interval, const TimerCallback &cb);

private:
    bool looping_;
    bool quit_;    /*atomic */
    ChannelList activeChannels_;

    std::shared_ptr<Poller> poller_;
    std::shared_ptr<TimerQueue> timerQueue_;
};


#endif //SABER_EVENTLOOP_H
