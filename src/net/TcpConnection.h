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
    typedef std::function<void()> Callback;
    
    TcpConnection(EventLoop* loop,
                  const std::string& nameArg,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr 
                  );
    ~TcpConnection();

public:
    void setConnectionCallback(const Callback& cb) { connectionCallback_ = cb; }
    void setMessageCallback(const Callback& cb) { messageCallback_ = cb; }
    void setWriteCompleteCallback(const Callback& cb) { writeCompleteCallback_ = cb; }
    void setCloseCallback(const Callback& cb) { closeCallback_ = cb; }

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

    Callback connectionCallback_;
    Callback messageCallback_;
    Callback writeCompleteCallback_;
    Callback closeCallback_;
    
    Buffer inputBuffer_;
    Buffer outputBuffer_;


    StateE state_;
};

#endif