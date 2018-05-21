#include "event_loop_thread.h"
#include "event_loop.h"

EventLoopThread::EventLoopThread()
  : loop_(NULL),
    exiting_(false),
    thread_(std::bind(&EventLoopThread::ThreadFunc, this))
{
}

EventLoopThread::~EventLoopThread()
{
  exiting_ = true;
  if (loop_ != NULL) // not 100% race-free, eg. threadFunc could be running callback_.
  {
    loop_->Quit();
    thread_.join();
  }
}

EventLoop* EventLoopThread::StartLoop()
{
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (loop_ == NULL)
    {
      cond_.wait(lock);
    }
  }

  return loop_;
}

void EventLoopThread::ThreadFunc()
{
  EventLoop loop;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }

  loop.Loop();
  //assert(exiting_);
  loop_ = NULL;
}

