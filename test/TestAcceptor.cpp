#include <unistd.h>
#include <iostream>

#include "net/EventLoop.h"
#include "net/Acceptor.h"
#include "base/InetAddress.h"

void newConnection(int sockfd, const InetAddress& peerAddr)
{
    printf("new connection:");
    ::write(sockfd, "how are yout?\n", 13);
    saber::sockets::close(sockfd);
}

int main() {
    InetAddress listenAddr(9981);
    EventLoop loop;
    Acceptor acceptor(&loop, listenAddr);
    acceptor.setNewConnectionCallback(newConnection);
    acceptor.listen();

    loop.loop();
}