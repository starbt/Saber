#include  "tcp_acceptor.h"
#include  "sockets_util.h"

TcpAcceptor::TcpAcceptor(EventLoop *loop, const InetAddress& listenAddr)
  : loop_(loop),
    accept_socket_(sockets::CreateNonblockingOrDie(AF_INET)),
    accept_channel_(loop, accept_socket_.sockfd()), 
    listenning_(false)
{
    accept_socket_.BindAddress(listenAddr);
    accept_channel_.set_readCallback(std::bind(&TcpAcceptor::HandleRead, this));
}

void TcpAcceptor::Listen()
{
    listenning_ = true;
    accept_socket_.Listen();
    accept_channel_.EnableReading();
}

void TcpAcceptor::HandleRead()  
{
    InetAddress peerAddr(0);
    int connfd = accept_socket_.Accept(&peerAddr);
    if (connfd >=0 ) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            sockets::Close(connfd);
        }
    }
}


