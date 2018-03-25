//
// Created by xcv on 17-12-18.
//

#ifndef SABER_CHANNEL_H
#define SABER_CHANNEL_H

#include <functional>
#include "EventLoop.h"

class EventLoop;

class Channel {
public:
    typedef std::function<void()> EventCallback;
    Channel(EventLoop *loop, int fd);

    void handleEvent();

    void setReadCallback(const EventCallback &cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void setRevents(int revt) { revents_ = revt; }
    bool isNoneEvent() const { return events_ == kNoneEvent; }

    void enableReading() { events_ |= kReadEvent;update(); }
    void enableWriting() { events_ |= kWriteEvent;update(); }
    void disableAll() { events_ = kNoneEvent;update(); }

    void remove();
    //for poller
    int index() { return index_; }
    void setIndex(int idx) { index_ = idx; }

    EventLoop *ownerLoop() { return loop_; }

private:
    void update();

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

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
};


#endif //SABER_CHANNEL_H
