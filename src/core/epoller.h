#ifndef SABER_POLLER_H
#define SABER_POLLER_H

#include "epoller.h"
#include <vector>
#include <map>

#include "timer_stamp.h"

class FdChannel;
class EventLoop;

struct pollfd;

class Epoller {
public:
    typedef std::vector<FdChannel*> ChannelList;
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, FdChannel*> ChannelMap;

    Epoller(EventLoop *loop);
    ~Epoller();

    Timestamp Poll(int timeout_ms, ChannelList *active_channels);
    //add or update
    void UpdateChannel(FdChannel *channel);
    void RemoveChannel(FdChannel *channel);

private:
    void FillActiveChannels(int num_events,  ChannelList *active_channels);

    EventLoop *owner_loop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};


#endif //SABER_POLLER_H
