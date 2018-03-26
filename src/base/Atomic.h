//
// Created by xcv on 17-12-20.
//

#ifndef SABER_ATOMIC_H
#define SABER_ATOMIC_H

#include <stdint.h>

namespace saber {
namespace detail {

template<typename T>
class AtomicIntegerT {
public:
    AtomicIntegerT()
            : value_(0) {}

    T get() {
        return __sync_val_compare_and_swap(&value_, 0, 0);
    }

    T getAndAdd(T x) {
        return __sync_fetch_and_add(&value_, x);
    }

    T addAndGet(T x) {
        return getAndAdd(x) + x;
    }

    T incrementaAndGet() {
        return addAndGet(1);
    }

    T decrementAndGet() {
        return addAndGet(-1);
    }

    void add(T x) {
        getAndAdd(x);
    }

    void increment() {
        incrementaAndGet();
    }

    void decrement() {
        decrement();
    }

    T getAndSet(T newValue) {
        return __sync_lock_test_and_set(&value_, newValue);
    }

private:
    volatile T value_;
};
}
    typedef detail::AtomicIntegerT<int32_t > AtomicInt32;
    typedef detail::AtomicIntegerT<int64_t > AtomicInt64;
}



#endif //SABER_ATOMIC_H