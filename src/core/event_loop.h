#ifndef SABER_EVENTLOOP_H
#define SABER_EVENTLOOP_H

#include <vector>
#include <memory>
#include "epoller.h"


class FdChannel;

class EventLoop {
public:
    typedef std::vector<FdChannel *> ChannelList;

    EventLoop();
    ~EventLoop();

public:
    void Loop();
    void Quit();

    void UpdateChannel(FdChannel *channel);
    void RemoveChannel(FdChannel *channel);

    void HandleRead();


private:
    bool looping_;
    bool quit_;    /*atomic */
    ChannelList active_channels_;

    std::shared_ptr<Epoller> poller_;
};


#endif //SABER_EVENTLOOP_H
