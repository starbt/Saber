#include <sys/poll.h>
#include <assert.h>
#include "Poller.h"
#include "Channel.h"


Poller::Poller(EventLoop *loop)
  : ownerLoop_(loop)
{ }

Poller::~Poller()
{ }

Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels) {
    int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
    Timestamp now(Timestamp::now());
    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        printf("nothing happend.\n");
    } else {
        printf("error:poll()");
    }
    return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) {
    for (auto iter = pollfds_.begin(); iter != pollfds_.end() && numEvents > 0; ++iter) {
        if (iter->revents > 0) {
            --numEvents;
            ChannelMap::const_iterator ch = channels_.find(iter->fd);
            Channel *channel = ch->second;
            channel->setRevents(iter->revents);
            activeChannels->push_back(channel);
        }
    }
}

void Poller::updateChannel(Channel *channel) {
    //add a new one
    if (channel->index() < 0) {
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;

        pollfds_.push_back(pfd);
        int idx = static_cast<int>(pollfds_.size()) - 1;
        channel->setIndex(idx);
        channels_[pfd.fd] = channel;
    } else {
        //update existing one.
        int idx = channel->index();
        struct pollfd &pfd = pollfds_[idx];
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if (channel->isNoneEvent()) {
            pfd.fd = -1;
        }
    }
}

void Poller::removeChannel(Channel *channel) {
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
        channels_[endId]->setIndex(idx);
        pollfds_.pop_back();
    }
}