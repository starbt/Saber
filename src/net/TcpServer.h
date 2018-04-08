#ifndef SABER_TCPSERVER_H
#define SABER_TCPSERVER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpConnection.h"

class EventLoop;

class TcpServer {
public:
    typedef std::function<void()> ConnectionCallback; 
    typedef std::function<void()> MessageCallback; 
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    enum Option {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop* loop, const InetAddress& listenAddr);
    ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& mb) { messageCallback_ = mb; }


private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    const string name_;
    const string ipPort_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    AtomicInt32 started_;
    int nextConnId_;
    ConnectionMap connections_;
};

#endif