//
// Created by xcv on 17-12-13.
//

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

void Socket::shutdownWrite() {
   sockets::shutdownWrite(sockfd_);
}

