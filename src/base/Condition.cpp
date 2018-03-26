//
// Created by xcv on 17-12-14.
//

#include <cstdio>
#include <cstdlib>
#include "Condition.h"

Condition::Condition(MutexLock &mutex) : mutex_(mutex) {
    CHECK(!pthread_cond_init(&cond_, NULL));
}

Condition::~Condition() {
   CHECK(!pthread_cond_destroy(&cond_));
}

void Condition::wait() {
    assert(mutex_.isLocking());  //wait前必须上锁
    CHECK(!pthread_cond_wait(&cond_, mutex_.getMutexPtr()));
    //pthread_cond_wait阻塞时释放锁， 返回时候自动加锁
    mutex_.restoreMutexStatus();
}
void Condition::notify() {
    CHECK(!pthread_cond_signal(&cond_));
}
void Condition::notifyAll() {
    CHECK(!pthread_cond_broadcast(&cond_));
}