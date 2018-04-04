#include <sys/type.h>
#include <sys/socket.h>
#include <memory.h>
#include "Socket.h"
#include "SocketsOps.h"

using namespace saber;

Socket::~Socket() {
    sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress &localaddr) {
    sockets::bindOrDie(sockfd_,localaddr.getSockAddr());
}

void Socket::listen() {
   sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress *peeraddr) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    int connfd = sockets::accept(sockfd_, &addr);
    if (connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::setKeepAlive(bool on) 
{
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &optval, 
                 static_cast<socklen_t>(sizeof optval));
}

void Socket::shutdownWrite() {
   sockets::shutdownWrite(sockfd_);
}

