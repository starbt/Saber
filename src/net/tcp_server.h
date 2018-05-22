#ifndef SABER_TCPSERVER_H
#define SABER_TCPSERVER_H

#include <functional>
#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "tcp_acceptor.h"
#include "inet_address.h"
#include "tcp_connection.h"
#include "time/timer_manager.h"

class EventLoop;
class EventLoopThreadPool;
class Buffer;

class TcpServer {
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void ()> TimerCallback;
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

    void set_thread_num(int num_threads);
    void set_connectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }
    void set_messageCallback(const MessageCallback& mb) { messageCallback_ = mb; }

    //一组定时回调函数
    void RunAt(const Timestamp &time, const TimerCallback &cb);
    void RunAfter(double delay, const TimerCallback &cb);
    void RunEvery(double interval, const TimerCallback &cb);


private:
    void NewConnection(int sockfd, const InetAddress& peer_addr);
    void RemoveConnection(const TcpConnectionPtr& conn);

private:
    EventLoop* loop_;
    const std::string name_;
    const std::string ip_port_;
    std::unique_ptr<TcpAcceptor> acceptor_;

    std::shared_ptr<EventLoopThreadPool> thread_pool_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    int next_conn_id_;

    std::mutex mutex_;
    ConnectionMap connections_;
    std::shared_ptr<TimerManager> timer_manager_;
};

#endif