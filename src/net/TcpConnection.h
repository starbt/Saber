#ifndef SABER_TCPCONNECTION_H
#define SABER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include <string>
#include "Buffer.h"
#include "InetAddress.h" 
#include "Socket.h"

class EventLoop; 
class Channel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback; 
    typedef std::function<void(const TcpConnectionPtr&, Buffer*)> MessageCallback; 
    typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

    TcpConnection(EventLoop* loop,
                  const std::string& nameArg,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr 
                  );

public:
    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void setCloseCallback(const CloseCallback& cb) { closeCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    void connectEstablished();
    void connectDestroyed();


    void send(const std::string& message);
    void send(Buffer *buf);
    void send(const void* data, size_t len);

    std::string name() { return name_; }

private:
    enum StateE { kConnecting, kConnected, kDisconnected, kDisconnecting };

    void setState(StateE s) { state_ = s; }
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

    void shutdown();

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
    CloseCallback closeCallback_;
    
    Buffer inputBuffer_;
    Buffer outputBuffer_;


    StateE state_;
};

#endif