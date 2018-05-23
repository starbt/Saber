#ifndef SABER_EVENTLOOP_H
#define SABER_EVENTLOOP_H

#include <vector>
#include <memory>
#include "epoller.h"


class FdChannel;

class EventLoop {
public:
    typedef std::vector<FdChannel *> ChannelList;
    //quit state
    enum QuitState {
        kSigNone,
        kSigQuit,
        kSigReload,
    };

    EventLoop();
    ~EventLoop();

public:
    void Loop();
    void Quit();

    void UpdateChannel(FdChannel *channel);
    void RemoveChannel(FdChannel *channel);

    void HandleRead();

    QuitState state() { return state_; }
    void set_state(QuitState s) { state_ = s; }

private:
    bool looping_;
    bool quit_;    
    QuitState state_;
    ChannelList active_channels_;

    std::shared_ptr<Epoller> poller_;
};


#endif //SABER_EVENTLOOP_H
