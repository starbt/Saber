#ifndef SABER_SIGNAL_MANAGER_H
#define SABER_SIGNAL_MANAGER_H

#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include "core/fd_channel.h"

class EventLoop;
class FdChannel;

class SignalManager {
public:
   SignalManager(EventLoop* loop);
   ~SignalManager();

   void HandleRead();

private:
   int CreateSigfd(); 
   void QuitHandle();
   void ReloadHandle();

private:
   EventLoop *loop_;
   int sigfd_;
   FdChannel sigfd_channel_;
};

#endif 