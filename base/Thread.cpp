//
// Created by xcv on 17-12-14.
//

#include <cstdio>
#include <cstdlib>
#include <syscall.h>
#include "Thread.h"

//注意在funcProxy函数之中调用,因为此时新线程才生成
pid_t Thread::getCurrentThreadPid() {
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

Thread::Thread(ThreadFunc func)
  : func_(func),
    isRunning_(false),
    threadId_(0),
    pid_(0)
{ }

Thread::~Thread() {
    if (isRunning_) {
        CHECK(!pthread_detach(threadId_));
    }
}

void *Thread::funcProxy(void *arg) {
    Thread *pt = static_cast<Thread*>(arg);
    //为线程pid赋值
    pt->pid_ = getCurrentThreadPid();
    pt->func_();
    return NULL;
}

void Thread::start() {
    CHECK(!pthread_create(&threadId_, NULL, Thread::funcProxy, this));
    isRunning_ = true;
}

//void *Thread::runProxy(void *arg) {
//    Thread *pt = static_cast<Thread*>(arg);
//    pt->run();
//    return NULL;
//}

//void Thread::start() {
//    CHECK(!pthread_create(&threadId_, NULL, Thread::runProxy, this));
//    isRunning_ = true;
//}

void Thread::join() {
    assert(isRunning_);

    CHECK(!pthread_join(threadId_, NULL));
    isRunning_ = false;
}