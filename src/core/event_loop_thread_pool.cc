#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"
#include <string>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, const std::string& name)
  : base_loop_(base_loop),
    name_(name),
    started_(false),
    num_threads_(0),
    next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

void EventLoopThreadPool::Start()
{
  started_ = true;
  for (int i = 0; i < num_threads_; ++i)
  {
    auto t = std::make_shared<EventLoopThread>(); 
    threads_.push_back(t);
    loops_.push_back(t->StartLoop());
  }
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
  EventLoop* loop = base_loop_;

  if (!loops_.empty())
  {
    // round-robin
    loop = loops_[next_];
    ++next_;
    if (static_cast<size_t>(next_) >= loops_.size())
    {
      next_ = 0;
    }
  }
  return loop;
}

EventLoop* EventLoopThreadPool::GetLoopForHash(size_t hashCode)
{
  EventLoop* loop = base_loop_;

  if (!loops_.empty())
  {
    loop = loops_[hashCode % loops_.size()];
  }
  return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::GetAllLoops()
{
  if (loops_.empty())
  {
    return std::vector<EventLoop*>(1, base_loop_);
  }
  else
  {
    return loops_;
  }
}