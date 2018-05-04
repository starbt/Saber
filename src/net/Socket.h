#ifndef SABER_SOCKET_H
#define SABER_SOCKET_H


#include "InetAddress.h"

class Socket {
public:
    explicit Socket(int sockfd)
      : sockfd_(sockfd)
    { }
    ~Socket();

    int fd() const { return sockfd_ ;};

    void bindAddress(const InetAddress &localaddr);

    void listen();

    int accept(InetAddress *peeraddr);

    void setKeepAlive(bool on);

    void shutdownWrite();

private:
    int sockfd_;
};


#endif //SABER_SOCKET_H
