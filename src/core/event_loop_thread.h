#ifndef SABER_EVENT_LOOP_THREAD_H
#define SABER_EVENT_LOOP_THREAD_H

#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>

class EventLoop;
class TcpServer;

class EventLoopThread
{
 public:

  EventLoopThread();
  ~EventLoopThread();

  EventLoop* StartLoop();

 private:
  void ThreadFunc();

  EventLoop* loop_;
  bool exiting_;
  std::thread thread_;
  std::mutex mutex_;
  std::condition_variable cond_;

};



#endif