//
// Created by xcv on 17-12-14.
//

#include <cstdio>
#include <cstdlib>
#include "MutexLock.h"

MutexLock::MutexLock() :isLocking_(false) {
    CHECK(!pthread_mutex_init(&mutex_, NULL));
}

MutexLock::~MutexLock() {
    assert(!isLocking());
    CHECK(!pthread_mutex_destroy(&mutex_));
}

void MutexLock::lock() {
    CHECK(!pthread_mutex_lock(&mutex_)); //先加锁再改变状态
    isLocking_ = true;
}

void MutexLock::unlock() {
    isLocking_ = false;
    CHECK(!pthread_mutex_unlock(&mutex_));  //先改变状态再解锁
}

