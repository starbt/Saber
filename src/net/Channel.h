#ifndef SABER_CHANNEL_H
#define SABER_CHANNEL_H

#include <functional>

class EventLoop;

class Channel {
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop *loop, int fd);
    void handleEvent();
    //for poller
    int index() { return index_; }
    void setIndex(int idx) { index_ = idx; }

public:
    void setReadCallback(const EventCallback &cb) { readCallback_ = cb; }
    void setWriteCallback(const EventCallback &cb) { writeCallback_ = cb; }
    void setErrorCallback(const EventCallback &cb) { errorCallback_ = cb; }
    void setCloseCallback(const EventCallback &cb) { closeCallback_ = cb; }

    void setRevents(int revt) { revents_ = revt; }
    void enableReading() { events_ |= kReadEvent;update(); }
    void enableWriting() { events_ |= kWriteEvent;update(); }
    void disableAll() { events_ = kNoneEvent;update(); }

    int fd() { return this->fd_; }
    int events() { return this->events_; }
    bool isNoneEvent() { return events_ == kNoneEvent; }

    void update();
    void remove();

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
    
    bool eventHandling_;

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};


#endif //SABER_CHANNEL_H
