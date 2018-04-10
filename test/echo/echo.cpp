#include "echo.h"
#include <functional>
#include <string>
#include <iostream>

EchoServer::EchoServer(EventLoop* loop, const InetAddress& listenAddr) 
  : server_(loop, listenAddr)
{
    server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&EchoServer::onMessage, this,std::placeholders::_1,std::placeholders::_2));
}

void EchoServer::start() 
{
    server_.start();
}

void EchoServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{
    std::cout << "connetion established." << std::endl;

}

void EchoServer::onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf)
{
    std::string msg(buf->retrieveAllAsString());
    conn->send(msg);
}