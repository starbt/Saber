#ifndef SABER_TCPCONNECTION_H
#define SABER_TCPCONNECTION_H

#include <memory>
#include <functional>
#include <string>
#include "buffer.h"
#include "inet_address.h" 
#include "socket.h"
#include "http/http_context.h"

class EventLoop; 
class FdChannel;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback; 
    typedef std::function<void(const TcpConnectionPtr&, Buffer*)> MessageCallback; 
    typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
    typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;

    TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& local_addr,
                  const InetAddress& peer_addr 
                  );

public:
    void set_connectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void set_messageCallback(const MessageCallback& cb) { messageCallback_ = cb; }
    void set_closeCallback(const CloseCallback& cb) { closeCallback_ = cb; }
    void set_writeCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    void ConnectEstablished();
    void ConnectDestroyed();

    bool Connected() const { return state_ == kConnected; }

    void Send(const std::string& message);
    void Send(Buffer *buf);
    void Send(const void* data, size_t len); 
    void set_context_ptr(std::shared_ptr<HttpContext> context) { 
        context_ptr_ = context;
    } 
    std::shared_ptr<HttpContext> context_ptr() {
        return context_ptr_;
    }

    std::string name() { return name_; }

    void Shutdown();

private:
    enum StateE { kConnecting, kConnected, kDisconnected, kDisconnecting };

    void set_state(StateE s) { state_ = s; }
    void HandleRead();
    void HandleWrite();
    void HandleClose();
    void HandleError();


    std::string name_;
    bool reading_;
    std::unique_ptr<Socket> socket_;
    std::unique_ptr<FdChannel> channel_;  
    InetAddress local_addr_;
    InetAddress peer_addr_;
    EventLoop* loop_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback closeCallback_;
    
    Buffer input_buffer_;
    Buffer output_buffer_;
    
    std::shared_ptr<HttpContext> context_ptr_;
    StateE state_;
};

#endif