#ifndef SABER_SOCKET_H
#define SABER_SOCKET_H


#include "inet_address.h"

class Socket {
public:
    explicit Socket(int sockfd)
      : sockfd_(sockfd)
    { }
    ~Socket();

    int sockfd() const { return sockfd_ ;};

    void BindAddress(const InetAddress &localaddr);

    void Listen();

    int Accept(InetAddress *peeraddr);

    void SetKeepAlive(bool on);

    void ShutdownWrite();

private:
    int sockfd_;
};


#endif //SABER_SOCKET_H
