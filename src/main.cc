#include <iostream>
#include "http/http_server.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "core/event_loop.h"
#include <list>

#include "manage/config_loader.h"

int main(int argc, char const *argv[])
{
    ConfigLoader::GetInstance()->Load("./config.json");
    return 0;
}


int main1()
{
    EventLoop loop;
    InetAddress inet_address(8090);
    HttpServer server(&loop, inet_address);
    server.set_thread_num(3);
    server.RunEvery(6, []() {
        std::cout << "hello world2." << std::endl;
    });
    server.Start();

    loop.Loop();
}
