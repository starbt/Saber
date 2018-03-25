cmake_minimum_required(VERSION 3.7)
project(saber)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp InetAddress.cpp InetAddress.h Socket.cpp Socket.h SocketsOps.cpp SocketsOps.h base/MutexLock.cpp base/MutexLock.h base/Condition.cpp base/Condition.h base/Thread.cpp base/Thread.h base/ThreadPool.cpp base/ThreadPool.h EventLoop.cpp EventLoop.h Channel.cpp Channel.h Poller.cpp Poller.h base/Timestamp.cpp base/Timestamp.h Timer.cpp Timer.h base/Atomic.h TimerQueue.cpp TimerQueue.h base/TimerId.cpp base/TimerId.h)
add_executable(saber ${SOURCE_FILES})

find_package(Threads REQUIRED)
target_link_libraries(saber Threads::Threads)