#include <unistd.h>
#include <iostream>

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"

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