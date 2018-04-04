#ifndef SABER_TCPSERVER_H
#define SABER_TCPSERVER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include "InetAddress.h"
#include "TcpConnection.h"

class EventLoop;

class TcpServer {
public:
    typedef std::function<void()> ConnectionCallback; 
    typedef std::function<void()> MessageCallback; 
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& mb) { messageCallback_ = mb; }

private:
    void newConnection(int sockfd, const InetAddress& peerAddr);

private:
    EventLoop* loop_;
    const string name_;
    const string ipPort_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    ConnectionMap connections_;
};

#endif