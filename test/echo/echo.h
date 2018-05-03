#ifndef SABER_TEST_ECHO_H
#define SABER_TEST_ECHO_H

#include "../../src/net/TcpServer.h"

class EchoServer
{
public:
    EchoServer(EventLoop* loop, InetAddress& listenAddr);

    void start();

private:
    void onConnection(const std::shared_ptr<TcpConnection>& conn);
    void onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf);
    
    TcpServer server_;
};

#endif