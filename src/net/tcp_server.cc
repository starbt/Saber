#include "tcp_server.h"
#include "sockets_util.h"
#include "buffer.h"
#include "core/event_loop_thread_pool.h"

TcpServer::TcpServer(EventLoop* loop,InetAddress& listen_addr)
 : loop_(loop),
   ip_port_(listen_addr.ToIpPort()),
   acceptor_(new TcpAcceptor(loop, listen_addr)),
   thread_pool_(new EventLoopThreadPool(loop, name_)),
   next_conn_id_(1),
   timer_manager_(new TimerManager(loop)),
   sig_manager_(new SignalManager(loop))
{
    acceptor_->set_newConnectionCallback(std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (ConnectionMap::iterator it(connections_.begin());it != connections_.end(); ++it) {

        TcpConnectionPtr conn(it->second);
        it->second.reset();
        conn->ConnectDestroyed();
    }
}

void TcpServer::Start()
{
    thread_pool_->Start();
    acceptor_->Listen();
}

void TcpServer::NewConnection(int sockfd, const InetAddress& peer_addr)
{
    EventLoop* io_loop = thread_pool_->GetNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ip_port_.c_str(), next_conn_id_);
    ++next_conn_id_;
    std::string connName = name_ + buf;

    InetAddress local_addr(sockets::GetLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(io_loop, connName, sockfd, local_addr, peer_addr));
    //need mutex lock here
    {
        std::lock_guard<std::mutex> lock(mutex_);
        connections_[connName] = conn;
    }
    conn->set_connectionCallback(connectionCallback_);
    conn->set_messageCallback(messageCallback_);
    conn->set_closeCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    conn->ConnectEstablished();
}


void TcpServer::RemoveConnection(const TcpConnectionPtr& conn)
{
    {
      std::lock_guard<std::mutex> lock(mutex_);
      size_t n = connections_.erase(conn->name());
    }
    conn->ConnectDestroyed();
}

void TcpServer::set_thread_num(int num_threads)
{
    thread_pool_->set_thread_num(num_threads);

}

void TcpServer::RunAt(const Timestamp &time, const TimerCallback &cb)
{
    timer_manager_->AddTimer(cb, time, 0.0);
}

void TcpServer::RunAfter(double delay, const TimerCallback &cb)
{
    Timestamp time(Timestamp::AddTime(Timestamp::Now(), delay));
    RunAt(time, cb);
}

void TcpServer::RunEvery(double interval, const TimerCallback &cb)
{
    Timestamp time(Timestamp::AddTime(Timestamp::Now(), interval));
    timer_manager_->AddTimer(cb, time, interval);
}
