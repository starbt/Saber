//
// Created by xcv on 17-12-18.
//

#include <sys/poll.h>
#include "Channel.h"
#include "EventLoop.h"


 const int Channel::kNoneEvent = 0;
 const int Channel::kReadEvent = POLLIN | POLLPRI;
 const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fd)
  : loop_(loop),
    fd_(fd),
    tied_(false),
    events_(0),
    revents_(0),
    index_(-1),
    eventHandling_(false)
{ }

void Channel::update() {
    loop_->updateChannel(this);
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::tie(const std::shared_ptr<TcpConnection>& tcpConnection)
{
    tie_ = tcpConnection;
    tied_ = true;
}

void Channel::handleEvent() {
    eventHandling_ = true;
    if (revents_ & POLLNVAL) {
       printf("handle_event POLLAVAL.\n");
    }
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        printf("handle_event POLLHUP.\n");
        if (closeCallback_) closeCallback_(); 
    }

    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) {
            readCallback_();
        }
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}