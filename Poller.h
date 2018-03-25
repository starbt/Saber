//
// Created by xcv on 17-12-18.
//

#ifndef SABER_POLLER_H
#define SABER_POLLER_H

#include <vector>
#include <map>
#include <poll.h>
#include "Channel.h"
#include "EventLoop.h"
#include "base/Timestamp.h"

class Channel;
class EventLoop;

struct pollfd;

class Poller {
public:
    typedef std::vector<Channel*> ChannelList;
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel*> ChannelMap;

    Poller(EventLoop *loop);
    ~Poller();

    Timestamp poll(int timeoutMs, ChannelList *activeChannels);
    //add or update
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);


private:
    void fillActiveChannels(int numEvents,
                            ChannelList *activeChannels) const;

    EventLoop *ownerLoop_;
    PollFdList pollfds_;
    ChannelMap channels_;
};


#endif //SABER_POLLER_H
