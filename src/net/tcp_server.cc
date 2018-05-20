#include "tcp_server.h"
#include "sockets_util.h"
#include "buffer.h"

TcpServer::TcpServer(EventLoop* loop,InetAddress& listen_addr)
 : loop_(loop),
   ip_port_(listen_addr.ToIpPort()),
   acceptor_(new TcpAcceptor(loop, listen_addr)),
   next_conn_id_(1)
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
    acceptor_->Listen();
}

void TcpServer::NewConnection(int sockfd, const InetAddress& peer_addr)
{
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ip_port_.c_str(), next_conn_id_);
    ++next_conn_id_;
    std::string connName = name_ + buf;

    InetAddress local_addr(sockets::GetLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, local_addr, peer_addr));
    connections_[connName] = conn;
    conn->set_connectionCallback(connectionCallback_);
    conn->set_messageCallback(messageCallback_);
    conn->set_closeCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    conn->ConnectEstablished();
}


void TcpServer::RemoveConnection(const TcpConnectionPtr& conn)
{
    size_t n = connections_.erase(conn->name());
    conn->ConnectDestroyed();
}
