#ifndef SABER_TEST_ECHO_H
#define SABER_TEST_ECHO_H

#include "../../src/net/TcpServer.h"

class EchoServer
{
public:
    EchoServer(EventLoop* loop, const InetAddress& listenAddr);

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buf);
    
    TcpServer server_;
};

#endif