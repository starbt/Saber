#ifndef SABER_EVENT_LOOP_THREAD_POOL_H
#define SABER_EVENT_LOOP_THREAD_POOL_H

#include <functional>
#include <memory>
#include <vector>
#include <string>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
 public:
  typedef std::function<void(EventLoop*)> ThreadInitCallback;

  EventLoopThreadPool(EventLoop* base_loop, const std::string& name);
  ~EventLoopThreadPool();
  void set_thread_num(int num_threads) { num_threads_ = num_threads; }
  void Start();

  /// round-robin
  EventLoop* GetNextLoop();

  /// with the same hash code, it will always return the same EventLoop
  EventLoop* GetLoopForHash(size_t hash_code);

  std::vector<EventLoop*> GetAllLoops();

  bool started() const
  { return started_; }

  const std::string& name() const
  { return name_; }

 private:

  EventLoop* base_loop_;
  std::string name_;
  bool started_;
  int num_threads_;
  int next_;
  std::vector<std::shared_ptr<EventLoopThread> > threads_;
  std::vector<EventLoop*> loops_;
};


#endif