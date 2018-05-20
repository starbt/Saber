#include "epoller.h"
#include <sys/poll.h>
#include <assert.h>
#include "fd_channel.h"


Epoller::Epoller(EventLoop *loop)
  : owner_loop_(loop)
{ }

Epoller::~Epoller()
{ }

Timestamp Epoller::Poll(int timeout_ms, ChannelList *active_channels) {
    int num_events = ::poll(&*pollfds_.begin(), pollfds_.size(), timeout_ms);
    Timestamp now(Timestamp::Now());
    if (num_events > 0) {
        FillActiveChannels(num_events, active_channels);
    } else if (num_events == 0) {
        // printf("nothing happend.\n");
    } else {
       // printf("error:poll()");
    }
    return now;
}

void Epoller::FillActiveChannels(int num_events, ChannelList *active_channels) {
    for (auto iter = pollfds_.begin(); iter != pollfds_.end() && num_events > 0; ++iter) {
        if (iter->revents > 0) {
            --num_events;
            ChannelMap::const_iterator ch = channels_.find(iter->fd);
            FdChannel *channel = ch->second;
            channel->set_revents(iter->revents);
            active_channels->push_back(channel);
        }
    }
}

void Epoller::UpdateChannel(FdChannel *channel) {
    //add a new one
    if (channel->index() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size()) - 1;
        channel->set_index(idx);
        channels_[pfd.fd] = channel;
    } else {
        //update existing one.
        int idx = channel->index();
        struct pollfd &pfd = pollfds_[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->IsNoneEvent()) {
            pfd.fd = -1;
        }
    }
}

void Epoller::RemoveChannel(FdChannel *channel) {
    int idx = channel->index();
    const struct pollfd &pfd = pollfds_[idx];
    size_t n = channels_.erase(channel->fd());
    //从vector中删除一个元素，如果是最后一个，直接删除，
    // 如果是中间的，与最后一个交换后删除
    if (static_cast<size_t>(idx) == pollfds_.size() - 1) {
        pollfds_.pop_back();
    } else {
        int endId = pollfds_.back().fd;
        std::swap(*(pollfds_.begin() + idx), *(pollfds_.end() - 1));
        channels_[endId]->set_index(idx);
        pollfds_.pop_back();
    }
}