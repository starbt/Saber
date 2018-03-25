//
// Created by xcv on 17-12-18.
//

#ifndef SABER_EVENTLOOP_H
#define SABER_EVENTLOOP_H

#include <vector>
#include <memory>
#include "base/Thread.h"
#include "Poller.h"
#include "Channel.h"
#include "TimerQueue.h"

class Channel;
class Poller;

class EventLoop {
public:
    typedef std::vector<Channel *> ChannelList;
    typedef std::function<void ()> Functor;
    typedef std::function<void ()> TimerCallback;

    EventLoop();
    ~EventLoop();

    bool isInLoopThread() const { return threadId_ == Thread::getCurrentThreadPid(); }
    void loop();
    void quit();
    void wakeup();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);

    // 在io线程中运行
    void runInLoop(const Functor &cb);
    void queueInLoop(const Functor &cb);

    void handleRead();
    void doPendingFunctors();

    //一组定时回调函数
    TimerId runAt(const Timer &time, const TimerCallback &cb);
    TimerId runAfter(double delay, const TimerCallback &cb);
    TimerId runEvery(double interval, const TimerCallback &cb);


private:
    bool looping_;
    const pid_t threadId_;
    bool quit_;    /*atomic */
    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    std::shared_ptr<Poller> poller_;
    std::shared_ptr<TimerQueue> timerQueue_;

    bool callingPendingFunctors_;
    int wakeupFd_;
    std::shared_ptr<Channel> wakeupChannel_;
    MutexLock mutex_;
    std::vector<Functor> pendingFunctors_;
};


#endif //SABER_EVENTLOOP_H
