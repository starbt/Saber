#include <iostream>
#include "http/http_server.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "core/event_loop.h"

int main()
{
    EventLoop loop;
    InetAddress inet_address(8090);
    HttpServer server(&loop, inet_address);
    server.set_thread_num(3);
    server.Start();

    loop.Loop();
}
