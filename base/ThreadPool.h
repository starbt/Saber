//
// Created by xcv on 17-12-15.
//

#ifndef SABER_THREADPOOL_H
#define SABER_THREADPOOL_H

#include <vector>
#include <deque>
#include <functional>

#include "MutexLock.h"
#include "Condition.h"
#include "Thread.h"

#include <memory>

class ThreadPool {
public:
    typedef std::function<void ()> Task;
    explicit ThreadPool(size_t threadNum, size_t maxQueueSize = 10);
    ~ThreadPool();

    void start();
    void stop();
    void addTask(const Task &task);
    size_t queueSize() const;

private:
    //线程中的运行函数,是一个循环
    void runInThread();
    Task takeTask();

    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    std::vector<std::shared_ptr<Thread>> threads_;
    std::deque<Task> queue_;
    bool running_;
    size_t threadNum_;
    size_t maxQueueSize_;
};


#endif //SABER_THREADPOOL_H
