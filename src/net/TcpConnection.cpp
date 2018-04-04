#include "TcpConnection.h"

#include <unistd.h>
#include "Socket.h"
#include "Channel.h"

TcpConnection::TcpConnection(EventLoop* loop,
                  const string& nameArg,
                  int sockfd,
                  const InetAddress& localAddr,
                  const InetAddress& peerAddr 
                  )
  :  loop_(loop),
     name_(nameArg),
     state_(kConnecting),
     reading_(true),
     socket_(new Socket(sockfd)),
     channel_(new Channel(loop, sockfd)),
     localAddr_(localAddr),
     peerAddr_(peerAddr)
{  
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this, _1));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));

    socket_->setKeepAlive(true);
}

void TcpConnection::handleRead() {
    char buf[65536];
    int n = ::read(channel_->fd(), buf, sizeof buf);
    if (n > 0) {
      messageCallback_(shared_from_this(), buf, n);
    } else if (n == 0) {
        handleClose();
    } else {
        handleError();
    }
}

void TcpConnection::handleClose() 
{
    channel_->disableAll();
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    printf("TcpConnection handleError.\n");
}

void TcpConnection::connectDestroyed()
{
    if (state_ == kConnected) 
    {
        setState(kDisconnected);
        channel_->disableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}