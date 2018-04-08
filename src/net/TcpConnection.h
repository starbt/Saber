#ifndef SABER_TCPCONNECTION_H
#define SABER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include <string>
#include "InetAddress.h" 
#include "Socket.h"
#include "Channel.h"

class EventLoop; 

class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
    typedef std::function<void()> ConnectionCallback;
    typedef std::function<void()> MessageCallback;
    typedef std::function<void()> CloseCallback;
    typedef std::function<void()> WriteCompleteCallback;

    TcpConnection(EventLoop* loop,
                  const string& nameArg,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr 
                  );
    ~TcpConnection();

public:
    void setConnectionCallback(const ConnectionCallback& cb) { ConnectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& mb) { messageCallback_ = mb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& wb) { writeCompleteCallback_ = wb; }
    void setCloseCallback(const closeCallback_& cb) { closeCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();

    void send(const std::string& message);

    std::string name() { return name_; }

private:
    enum StateE { kConnecting, kConnected, kDisconnected, kDisconnecting };

    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    std::string name_;
    bool reading_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;  
    InetAddress localAddr_;
    InetAddress peerAddr_;
    EventLoop* loop_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    Closecallback closeCallback_;

    StateE state_;
};

#endif