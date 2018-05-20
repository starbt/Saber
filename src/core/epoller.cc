#include "epoller.h"
#include <sys/epoll.h>
#include <assert.h>
#include <unistd.h>
#include <strings.h>
#include "fd_channel.h"

const int Epoller::kNew = -1; 
const int Epoller::kAdded = 1;
const int Epoller::kDeleted = 2;
const int Epoller::kInitEventListSize = 16;

Epoller::Epoller(EventLoop *loop)
  : owner_loop_(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{ }

Epoller::~Epoller()
{ 
    ::close(epollfd_);
}

Timestamp Epoller::Poll(int timeout_ms, ChannelList *active_channels) {
    int numEvents = ::epoll_wait(epollfd_,
                                &*events_.begin(),
                                static_cast<int>(events_.size()),
                                timeout_ms);
    int savedErrno = errno;
    Timestamp now(Timestamp::Now());
    if (numEvents > 0)
    {
        FillActiveChannels(numEvents, active_channels);
        if (static_cast<size_t>(numEvents) == events_.size())
        {
         events_.resize(events_.size()*2);
        }
    }
    else if (numEvents == 0)
    {
        // printf("nothing happend.\n");
    }
    else
    {
        // error happens, log uncommon ones
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            printf("EPollPoller::poll()");
        }
    }
    return now;
}

void Epoller::FillActiveChannels(int num_events, ChannelList *active_channels) {
    for (int i = 0; i < num_events; ++i)
    {
        FdChannel* channel = static_cast<FdChannel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        active_channels->push_back(channel);
    }
}

void Epoller::UpdateChannel(FdChannel *channel) {
    const int index = channel->index();
    if (index == kNew || index == kDeleted)
    {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew)
        {
          channels_[fd] = channel;
        }
        else // index == kDeleted
        {
            //assert(channels_.find(fd) != channels_.end());
            //assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        Update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // update existing one with EPOLL_CTL_MOD/DEL
        if (channel->IsNoneEvent())
        {
           Update(EPOLL_CTL_DEL, channel);
           channel->set_index(kDeleted);
        }
        else
        {
          Update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Epoller::RemoveChannel(FdChannel *channel) {
    int fd = channel->fd();
    int index = channel->index();
    channels_.erase(fd);
    if (index == kAdded)
    {
        Update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}


void Epoller::Update(int operation, FdChannel* channel) {
    struct epoll_event event;
    bzero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        printf("epoll_ctl error.\n");
    }
}
