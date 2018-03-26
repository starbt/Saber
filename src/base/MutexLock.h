//
// Created by xcv on 17-12-14.
//

#ifndef SABER_MUTEXLOCK_H
#define SABER_MUTEXLOCK_H

//int pthread_mutex_init(pthread_mutex_t  *mutex,  const  pthread_mutexattr_t *mutexattr);
//int pthread_mutex_lock(pthread_mutex_t *mutex);
//int pthread_mutex_trylock(pthread_mutex_t *mutex);
//int pthread_mutex_unlock(pthread_mutex_t *mutex);
//int pthread_mutex_destroy(pthread_mutex_t *mutex);

#include <zconf.h>
#include <pthread.h>
#include <assert.h>

#define CHECK(exp) \
    if (!exp) \
    { \
        fprintf(stderr,"File:%s, Line:%d Exp:[" #exp "] is true,abort.\n",__FILE__,__LINE__);abort(); \
    }

class MutexLock {
    friend class Condition;
public:
    MutexLock();
    ~MutexLock();
    void lock();
    void unlock();
    bool isLocking() const { return isLocking_; }

    pthread_mutex_t *getMutexPtr() { return &mutex_; }

private:
    void restoreMutexStatus() {
        isLocking_ = true;
    }
    pthread_mutex_t mutex_;
    bool isLocking_;
};


class MutexLockGuard {
public:
    MutexLockGuard(MutexLock &mutex)
      : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard() {
        mutex_.unlock();
    }

private:
    MutexLock  &mutex_;
};

#endif //SABER_MUTEXLOCK_H
