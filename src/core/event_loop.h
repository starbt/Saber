#ifndef SABER_EVENTLOOP_H
#define SABER_EVENTLOOP_H

#include <vector>
#include <memory>
#include "epoller.h"

#include "timer_manager.h"

class FdChannel;

class EventLoop {
public:
    typedef std::vector<FdChannel *> ChannelList;
    typedef std::function<void ()> TimerCallback;

    EventLoop();
    ~EventLoop();

public:
    void Loop();
    void Quit();

    void UpdateChannel(FdChannel *channel);
    void RemoveChannel(FdChannel *channel);

    void HandleRead();

    //一组定时回调函数
    TimerId RunAt(const Timestamp &time, const TimerCallback &cb);
    TimerId RunAfter(double delay, const TimerCallback &cb);
    TimerId RunEvery(double interval, const TimerCallback &cb);

private:
    bool looping_;
    bool quit_;    /*atomic */
    ChannelList active_channels_;

    std::shared_ptr<Epoller> poller_;
    std::shared_ptr<TimerManager> timer_manager_;
};


#endif //SABER_EVENTLOOP_H
