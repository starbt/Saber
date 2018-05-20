#include "sockets_util.h"

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <endian.h>

typedef struct sockaddr SA;

const struct sockaddr *sockets::sockaddr_cast(const struct sockaddr_in *addr) {
    return reinterpret_cast<const struct sockaddr*>(addr);
}

const struct sockaddr_in *sockets::sockaddr_in_case(const struct sockaddr *addr) {
    return reinterpret_cast<const struct sockaddr_in*>(addr);
}

int sockets::CreateNonblockingOrDie(sa_family_t family) {
    int sockfd = ::socket(family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) {
        printf("error");
    }
    return sockfd;
}

void sockets::BindOrDie(int sockfd, const struct sockaddr_in *addr) {
    int ret = ::bind(sockfd, sockaddr_cast(addr), sizeof(*addr));
    if (ret < 0) {
        printf("bind error.\n");
    }
}

void sockets::ListenOrDie(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        printf("listen error.\n");
    }
}

int sockets::Accept(int sockfd, struct sockaddr_in *addr) {
    socklen_t addrlen = static_cast<socklen_t>(sizeof(* addr));
    int connfd = ::accept4(sockfd, const_cast<struct sockaddr*>(sockaddr_cast(addr)),
                           &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        printf("error:accept\n");
    }
    return connfd;
}

struct sockaddr_in sockets::GetLocalAddr(int sockfd) {
    struct sockaddr_in localaddr;
    bzero(&localaddr, sizeof localaddr);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
    ::getsockname(sockfd, const_cast<struct sockaddr*>(sockaddr_cast(&localaddr)), &addrlen);
    return localaddr;
}

void sockets::ToIp(char* buf, size_t size, const struct sockaddr_in* addr) {
    ::inet_ntop(AF_INET, &addr->sin_addr, buf, static_cast<socklen_t>(size));
}

void sockets::ToIpPort(char* buf, size_t size, const struct sockaddr_in* addr) {
    ToIp(buf, size, addr);
    size_t end = ::strlen(buf);
    uint16_t port = be16toh(addr->sin_port);
    snprintf(buf + end, size - end, ":%u", port);
}


int sockets::Connect(int sockfd, const struct sockaddr_in *addr) {
    return ::connect(sockfd, const_cast<struct sockaddr*>(sockaddr_cast(addr)), sizeof(*addr));
}

ssize_t sockets::Read(int sockfd, void *buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t sockets::Write(int sockfd, const void *buf, size_t count) {
    return ::write(sockfd, buf, count);
}

ssize_t sockets::Readv(int sockfd, const struct iovec *iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

void sockets::Close(int sockfd) {
    if (::close(sockfd) < 0) {
        printf("error: socket close.\n");
    }
}

void sockets::ShutdownWrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        printf("error:shutdown.\n");
    }
}
