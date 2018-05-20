#ifndef SABER_ACCEPTOR_H
#define SABER_ACCEPTOR_H

#include "inet_address.h"
#include <functional> 
#include "core/fd_channel.h"
#include "socket.h"

class EventLoop;

class TcpAcceptor 
{
public:
    typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    TcpAcceptor(EventLoop *loop, const InetAddress& listenAddr);
    void set_newConnectionCallback(const NewConnectionCallback &cb) {  newConnectionCallback_ = cb;  } 

    bool listenngin() { return listenning_; }
    void Listen();
    
private:
    void HandleRead();

    EventLoop *loop_;
    Socket accept_socket_;
    FdChannel accept_channel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};

#endif
