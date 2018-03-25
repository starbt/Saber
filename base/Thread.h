//
// Created by xcv on 17-12-14.
//

#ifndef SABER_THREAD_H
#define SABER_THREAD_H

//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
//                   void *(*start_routine) (void *), void *arg);

#include "MutexLock.h"
#include "Condition.h"
#include <pthread.h>
#include <unistd.h>
#include <functional>

// 这里必须要用静态函数或者友元函数，没有this指针，
// 这样才能与pthread_creat()中的参数匹配

class Thread {
public:
    typedef std::function<void ()> ThreadFunc;
    //得到当前线程的pid
    static pid_t getCurrentThreadPid();

   // Thread() : isRunning_(false), threadId_(0) {}
    explicit Thread(ThreadFunc func);
    virtual ~Thread();
    void start();
    void join();
    // virtual void run() = 0;
    pthread_t getThreadId() const { return threadId_; }
    pid_t getPid() const { return pid_; }

private:
    //以继承重写run方法的方式
    //static void *runProxy(void *arg);
    //以函数对象的方式
    static void *funcProxy(void *arg);

    pthread_t threadId_;
    bool isRunning_;
    ThreadFunc func_;
    //此pid是操作系统分配的，唯一性
    pid_t pid_;
};

#endif //SABER_THREAD_H
