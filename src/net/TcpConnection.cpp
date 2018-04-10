#include "TcpConnection.h"

#include <unistd.h>
#include <string>
#include "Socket.h"
#include "Channel.h"
#include "SocketsOps.h"
#include "Buffer.h"

#include <error.h>

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

void TcpConnection::connectEstablished() {
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed() {
    if (state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}
        
void TcpConnection::handleRead() {
    int saveError = 0;
    ssize_t n = inputBuffer_.readFd(channel_->fd(), &saveError);
    if (n > 0) {
      messageCallback_(shared_from_this(), &inputBuffer_);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = saveError;
        handleError();
    }
}

void TcpConnection::handleWrite() {
    if (channel_->isWriting()) {
        ssize_t n = sockets::write(channel_->fd(), 
                                   outputBuffer_.peek(), 
                                   outputBuffer_.readableBytes());
        if (n > 0) {
            if (n < outputBuffer_.readableBytes()) {
                readerIndex_ += n;
            } else {
               readerIndex_ = Buffer::kCheapPrepend;
               writeIndex_ =  Buffer::kCheapPrepend; 
            }

            if(outputBuffer_.readableBytes() == 0) 
            {
                channel_->disableWriting();
                if (writeCompleteCallback_) {
                    writeCompleteCallback_(std::shared_from_this());
                }
                if (state_ == kDisconnecting) 
                {
                    shutdown();
                }
            }
        }  
    }
}

void TcpConnection::handleClose() 
{
    setState(kDisconnected);
    channel_->disableAll();

    ConnectionCallback(shared_from_this())
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

void TcpConnection::send(Buffer* buf) {
    send(buf->peek(), buf->readableBytes());
    buf->readerIndex_ = kCheapPrepend;
    buf->writerIndex_ = kCheapPrepend;
}

void TcpConnection::send(const std::string& message)
{
    send(message.c_str(), message.size());
}

void TcpConnection::send(const void* data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
        nwrote =  sockets::write(channel_->fd(), data, len);
        if (nwrote >=0 ) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                writeCompleteCallback_(std::shared_from_this());
            }
        } else {
            nwrote = 0;
            if (errno != EWOULDBLOCK) {
                if (errno == EPIPE || errno == ECONNRESET) {
                    faultError = true;
                }
            }
        }
    }

    if (!faultError && remaining > 0) {
        outputBuffer_.append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->isWriting) {
            channel_->enableWriting();
        }
    }
}


void TcpConnection::shutdown()
{
    if (!channel_->isWriting()) 
    {
        socket_->shutdownWrite();
    }
}