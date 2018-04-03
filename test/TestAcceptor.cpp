#include <unistd.h>
#include <iostream>

#include "../src/net/EventLoop.h"
#include "../src/net/Acceptor.h"
#include "../src/net/InetAddress.h"

#include "../src/net/SocketsOps.h"

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
