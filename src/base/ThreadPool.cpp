//
// Created by xcv on 17-12-15.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadNum, size_t maxQueueSize)
  : mutex_(),
    notEmpty_(mutex_),
    notFull_(mutex_),
    running_(false),
    threadNum_(threadNum),
    maxQueueSize_(maxQueueSize)
{ }

ThreadPool::~ThreadPool() {
    if (running_) {
        stop();
    }
}

size_t ThreadPool::queueSize() const {
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

//添加任务到队列
void ThreadPool::addTask(const Task &task) {
    //防止如果没有调用start（）
    if (threads_.empty()) {
        task();
    } else {
        MutexLockGuard lock(mutex_);
        while (queue_.size() >= maxQueueSize_) {
           notFull_.wait();
        }
        //要不要判断为非满？
        if (queue_.size() < maxQueueSize_) {
            queue_.push_back(task);
            notEmpty_.notify();
        }
    }
}

//从队列中取出任务
ThreadPool::Task ThreadPool::takeTask() {
    MutexLockGuard lock(mutex_);
    while (queue_.empty() && running_) {
        notEmpty_.wait();
    }
    Task task;
    //要不要加判断为空？？
    if (!queue_.empty()) {
        task = queue_.front();
        queue_.pop_front();
        notFull_.notify();
    }
    return task;
}

//线程中执行循环任务，没有异常
void ThreadPool::runInThread() {
    while (running_) {
        Task task(takeTask());
        if (task) {
            task();
        }
    }
}

void ThreadPool::start() {
    assert(threads_.empty());
    running_ = true;
    for (int i = 0; i < threadNum_; ++i) {
        threads_.push_back(std::make_shared<Thread>(std::bind(&ThreadPool::runInThread, this)));
        threads_[i]->start();
    }
}

void ThreadPool::stop() {
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
        //不需要notFull.notifyAll()??
    }
    for (auto thread : threads_) {
        thread->join();
    }
}




