#ifndef SABER_TCPSERVER_H
#define SABER_TCPSERVER_H

#include <functional>
#include <map>
#include <string>
#include <memory>

#include "tcp_acceptor.h"
#include "inet_address.h"
#include "tcp_connection.h"

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

   TcpServer(EventLoop* loop, InetAddress& listen_addr);
   ~TcpServer();

    void Start();

    void set_connectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void set_messageCallback(const MessageCallback& mb) { messageCallback_ = mb; }


private:
    void NewConnection(int sockfd, const InetAddress& peer_addr);
    void RemoveConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    const std::string name_;
    const std::string ip_port_;
    std::unique_ptr<TcpAcceptor> acceptor_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    int next_conn_id_;
    ConnectionMap connections_;
};

#endif