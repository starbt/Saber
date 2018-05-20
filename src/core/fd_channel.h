#ifndef SABER_CHANNEL_H
#define SABER_CHANNEL_H

#include <functional>
#include <memory>
#include "net/tcp_connection.h"

class EventLoop;

class FdChannel {
public:
    typedef std::function<void()> EventCallback;

    FdChannel(EventLoop *loop, int fd);
    void HandleEvent();
    //for poller
    int index() { return index_; }
    void set_index(int idx) { index_ = idx; }

public:
    void set_readCallback(const EventCallback &cb) { readCallback_ = cb; }
    void set_writeCallback(const EventCallback &cb) { writeCallback_ = cb; }
    void set_errorCallback(const EventCallback &cb) { errorCallback_ = cb; }
    void set_closeCallback(const EventCallback &cb) { closeCallback_ = cb; }

    void set_revents(int revt) { revents_ = revt; }

    void EnableReading() { events_ |= kReadEvent;Update(); }
    void EnableWriting() { events_ |= kWriteEvent;Update(); }
    void DisableAll() { events_ = kNoneEvent;Update(); }
    void DisableWriting() { events_ &= ~kWriteEvent; Update(); }

    int fd() { return fd_; }
    int events() { return events_; }
    bool IsNoneEvent() { return events_ == kNoneEvent; }
    bool IsWriting() { return events_ & kWriteEvent; }
    bool IsReading() { return events_ & kReadEvent; }

    void Tie(const std::shared_ptr<TcpConnection>& tcpConnection);
    void Update();
    void Remove();

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    const int fd_;

    //events_所关心的事件
    //revents_目前所活动的事件
    int events_;
    int revents_;
    int index_;
    
    bool event_handling_;
    std::weak_ptr<TcpConnection> tie_;
    bool tied_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};


#endif //SABER_CHANNEL_H
