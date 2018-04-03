//
// Created by xcv on 17-12-13.
//

#include "SocketsOps.h"

#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

using namespace saber;


typedef struct sockaddr SA;

const struct sockaddr *sockets::sockaddr_cast(const struct sockaddr_in *addr) {
    return reinterpret_cast<const struct sockaddr*>(addr);
}

const struct sockaddr_in *sockaddr_in_case(const struct sockaddr *addr) {
    return reinterpret_cast<const struct sockaddr_in*>(addr);
}

int sockets::createNonblockingOrDie(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        printf("error");
    }
    return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in *addr) {
    int ret = ::bind(sockfd, sockaddr_cast(addr), sizeof(*addr));
    if (ret < 0) {
        printf("bind error.\n");
    }
}

void sockets::listenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        printf("listen error.\n");
    }
}

int sockets::accept(int sockfd, struct sockaddr_in *addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(* addr));
    int connfd = ::accept4(sockfd, const_cast<struct sockaddr*>(sockaddr_cast(addr)),
                           &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        printf("error:accept\n");
    }
    return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr_in *addr) {
    return ::connect(sockfd, sockaddr_cast(addr), sizeof(*addr));
}

ssize_t sockets::read(int sockfd, void *buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t sockets::write(int sockfd, const void *buf, size_t count) {
    return ::write(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

void sockets::close(int sockfd) {
    if (::close(sockfd) < 0) {
        printf("error: socket close.\n");
    }
}

void sockets::shutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        printf("error:shutdown.\n");
    }
}

