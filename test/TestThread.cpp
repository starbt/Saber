#include <iostream>

#include "base/Thread.h"

MutexLock mutex;
Condition cond(mutex);
int count = 0;

class IncThread : public Thread{
public:
    IncThread(int id) : id_(id) { }

    void run() {
        for (int i = 0; i < 10; ++i) {

            {
                MutexLockGuard lock(mutex);
                count++;
                if (count == 12) {
                    cond.notify();
                }
                std::cout << "thread" << id_ << "count:" << count << std::endl;
            }
            sleep(1);
        }
    }

private:
    int id_;
};

class WatchThread : public Thread {
public:
    WatchThread(int id) : id_(id) { }

    void run() {
        MutexLockGuard lock(mutex);
        while (count < 12) {
            cond.wait();
        }
        count += 100;
        std::cout << "thread" << id_ << "count:" << count << std::endl;
    }

private:
    int id_;
};

int main() {
    IncThread t1(1);
    IncThread t2(2);
    WatchThread t3(3);

    t1.start();
    t2.start();
    t3.start();

    t1.join();
    t2.join();
    t3.join();

    return 0;
}