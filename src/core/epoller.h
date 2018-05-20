#ifndef SABER_POLLER_H
#define SABER_POLLER_H

#include "epoller.h"
#include <vector>
#include <map>

#include "timer_stamp.h"

class FdChannel;
class EventLoop;

struct epoll_event;

class Epoller {
public:
    typedef std::vector<FdChannel*> ChannelList;
    typedef std::vector<struct epoll_event> EventList;
    typedef std::map<int, FdChannel*> ChannelMap;

    static const int kNew;
    static const int kAdded;
    static const int kDeleted;
    static const int kInitEventListSize;

    Epoller(EventLoop *loop);
    ~Epoller();

    Timestamp Poll(int timeout_ms, ChannelList *active_channels);
    //add or update
    void UpdateChannel(FdChannel *channel);
    void RemoveChannel(FdChannel *channel);

private:
    void FillActiveChannels(int num_events,  ChannelList *active_channels);
    void Update(int operation, FdChannel* channel);
    
    EventLoop *owner_loop_;
    int epollfd_;
    EventList events_;
    ChannelMap channels_;
};


#endif //SABER_POLLER_H
