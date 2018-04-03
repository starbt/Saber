#ifndef SABER_ACCEPTOR_H
#define SABER_ACCEPTOR_H

#include "InetAddress.h"
#include <functional> 

class EventLoop;

class Acceptor 
{
public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop *loop, const InetAddress& listenAddr);
    void setNewConnectionCallback(const NewConnectionCallback &cb) {  newConnectionCallback_ = cb;  } 

    bool listenngin() { return listenning_; }
    void listen();
    
private:
    void handleRead();

    EventLoop *loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

#endif