#ifndef SABER_TCPSERVER_H
#define SABER_TCPSERVER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include "Acceptor.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include <memory>

class EventLoop;
class Buffer;

class TcpServer {
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback; 
    typedef std::function<void(const TcpConnectionPtr&, Buffer*)> MessageCallback; 
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    enum Option {
        kNoReusePort,
        kReusePort,
    };

   TcpServer(EventLoop* loop, InetAddress& listenAddr);
   ~TcpServer();

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& mb) { messageCallback_ = mb; }


private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    const std::string name_;
    const std::string ipPort_;
    std::unique_ptr<Acceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    int nextConnId_;
    ConnectionMap connections_;
};

#endif