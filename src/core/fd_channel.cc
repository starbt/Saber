#include "fd_channel.h"
#include <sys/poll.h>

#include "event_loop.h"


 const int FdChannel::kNoneEvent = 0;
 const int FdChannel::kReadEvent = POLLIN | POLLPRI;
 const int FdChannel::kWriteEvent = POLLOUT;

FdChannel::FdChannel(EventLoop *loop, int fd)
  : loop_(loop),
    fd_(fd),
    tied_(false),
    events_(0),
    revents_(0),
    index_(-1),
    event_handling_(false)
{ }

void FdChannel::Update() {
    loop_->UpdateChannel(this);
}

void FdChannel::Remove() {
    loop_->RemoveChannel(this);
}

void FdChannel::Tie(const std::shared_ptr<TcpConnection>& tcpConnection)
{
    tie_ = tcpConnection;
    tied_ = true;
}

void FdChannel::HandleEvent() {
    event_handling_ = true;
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
    event_handling_ = false;
}