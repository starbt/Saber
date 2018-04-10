#include "echo.h"

#include "../../src/net/EventLoop.h"
#include <unistd.h>

int main()
{
    EventLoop loop;
    InetAddress listenAddr(8088);
    EchoServer server(&loop, listenAddr);
    server_.start();
    loop.loop();
}