#include "socket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <memory.h>
#include "sockets_util.h"

Socket::~Socket() {
    sockets::Close(sockfd_);
}

void Socket::BindAddress(const InetAddress &localaddr) {
    sockets::BindOrDie(sockfd_,localaddr.GetSockAddr());
}

void Socket::Listen() {
   sockets::ListenOrDie(sockfd_);
}

int Socket::Accept(InetAddress *peeraddr) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::Accept(sockfd_, &addr);
    if (connfd >= 0) {
        peeraddr->set_addr(addr);
    }
    return connfd;
}

void Socket::SetKeepAlive(bool on) 
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, 
                 static_cast<socklen_t>(sizeof optval));
}

void Socket::ShutdownWrite() {
   sockets::ShutdownWrite(sockfd_);
}

