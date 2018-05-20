#include "tcp_connection.h"

#include <unistd.h>
#include <string>
#include <memory>
#include <error.h>

#include "socket.h"
#include "core/fd_channel.h"
#include "sockets_util.h"
#include "buffer.h"

TcpConnection::TcpConnection(EventLoop* loop,
                  const std::string& name,
                  int sockfd,
                  const InetAddress& local_addr,
                  const InetAddress& peer_addr 
                  )
  :  loop_(loop),
     name_(name),
     state_(kConnecting),
     reading_(true),
     socket_(new Socket(sockfd)),
     channel_(new FdChannel(loop, sockfd)),
     local_addr_(local_addr),
     peer_addr_(peer_addr)
{  
    channel_->set_readCallback(std::bind(&TcpConnection::HandleRead, this));
    channel_->set_writeCallback(std::bind(&TcpConnection::HandleWrite, this));
    channel_->set_closeCallback(std::bind(&TcpConnection::HandleClose, this));
    channel_->set_errorCallback(std::bind(&TcpConnection::HandleError, this));

    socket_->SetKeepAlive(true);
}

void TcpConnection::ConnectEstablished() {
    set_state(kConnected);
    channel_->Tie(shared_from_this());
    channel_->EnableReading();

    connectionCallback_(shared_from_this());
}

        
void TcpConnection::HandleRead() {
    int saveError = 0;
    ssize_t n = input_buffer_.ReadFd(channel_->fd(), &saveError);
    if (n > 0) {
      messageCallback_(shared_from_this(), &input_buffer_);
    } else if (n == 0) {
        HandleClose();
    } else {
        errno = saveError;
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    if (channel_->IsWriting()) {
        ssize_t n = sockets::Write(channel_->fd(), 
                                   output_buffer_.Peek(), 
                                   output_buffer_.ReadableBytes());
        if (n > 0) {
            output_buffer_.Retrieve(n);

            if(output_buffer_.ReadableBytes() == 0) 
            {
                channel_->DisableWriting();
                if (writeCompleteCallback_) {
                    writeCompleteCallback_(shared_from_this());
                }
                if (state_ == kDisconnecting) 
                {
                    Shutdown();
                }
            }
        }  
    }
}

void TcpConnection::HandleClose() 
{
    set_state(kDisconnected);
    channel_->DisableAll();
    
    TcpConnectionPtr guardThis(shared_from_this());
    connectionCallback_(guardThis);
    closeCallback_(guardThis);
}

void TcpConnection::HandleError()
{
    printf("TcpConnection handleError.\n");
}

void TcpConnection::ConnectDestroyed()
{
    if (state_ == kConnected) 
    {
        set_state(kDisconnected);
        channel_->DisableAll();
        connectionCallback_(shared_from_this());
    }
    channel_->Remove();
}


void TcpConnection::Send(Buffer* buf) {
    Send(buf->Peek(), buf->ReadableBytes());
    buf->RetrievePrepend();
}

void TcpConnection::Send(const std::string& message)
{
    Send(message.c_str(), message.size());
}


void TcpConnection::Send(const void* data, size_t len)
{
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if (!channel_->IsWriting() && output_buffer_.ReadableBytes() == 0) {
        nwrote = sockets::Write(channel_->fd(), data, len);
        if (nwrote >=0 ) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCompleteCallback_) {
                writeCompleteCallback_(shared_from_this());
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
        output_buffer_.Append(static_cast<const char*>(data) + nwrote, remaining);
        if (!channel_->IsWriting()) {
            channel_->EnableWriting();
        }
    }
}


void TcpConnection::Shutdown()
{
    if (!channel_->IsWriting()) 
    {
        socket_->ShutdownWrite();
    }
}