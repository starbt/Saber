//
// Created by xcv on 17-12-14.
//

#ifndef SABER_CONDITION_H
#define SABER_CONDITION_H

//pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//静态初始化
//
//int pthread_cond_init(pthread_cond_t    *cond,    pthread_condattr_t *cond_attr);//动态初始化
//int pthread_cond_signal(pthread_cond_t *cond);
//int pthread_cond_broadcast(pthread_cond_t *cond);
//int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
//int pthread_cond_timedwait(pthread_cond_t   *cond,    pthread_mutex_t *mutex, const struct timespec *abstime);
//int pthread_cond_destroy(pthread_cond_t *cond);

#include "MutexLock.h"

class MutexLock;

class Condition {
public:
    Condition(MutexLock &mutex);
    ~Condition();
    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t cond_;
    MutexLock &mutex_;
};


#endif //SABER_CONDITION_H
