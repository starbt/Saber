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

    void setRevents(int revt) { revents_ = revt; }
    void enableReading() { events_ |= kReadEvent;update(); }
    void enableWriting() { events_ |= kWriteEvent;update(); }
    void disableAll() { events_ = kNoneEvent;update(); }

private:
    void update();
    void remove();

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
