#include "TcpServer.h"


TcpServer::TcpServer(EventLoop* loop, 
                     const InetAddress& listenAddr,
                     const string& nameArg,
                     Option option)
    : loop_(loop),
      ipPort_(listenAddr.toIpPort()),
      name_(nameArg),
      acceptor_(new Acceptor(loop, listenAddr)),
      nextConnId_(1)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for (ConnectionMap::iterator it(connections_.begin());
        it != connections_.end(); ++it) {
        TcpConnectionPtr conn(it->second);
        it->second.reset();
        conn.connectDestroyed();
    }
}

void TcpServer::start()
{
    acceptor_->listen();
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));

    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    conn->connectEstablished();
}


void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    size_t n = connections_.erase(conn->name());
    conn->connectDestroyed();
}
