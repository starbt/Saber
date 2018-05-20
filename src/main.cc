#include <iostream>
#include "http/http_server.h"
#include "http/http_response.h"
#include "http/http_request.h"
#include "core/event_loop.h"

void OnRequest(const HttpRequest& req, HttpResponse* resp)
{
    //std::cout << "Headers " << req.method_string() <<  " " << req.path() << std::endl;
    //auto headers = req.headers();
    //for (auto it = headers.begin(); it != headers.end(); ++it)
    //{
    //    std::cout << it->first << ":" << it->second << std::endl;
    //}
    if (req.path() == "/") {
        resp->set_status_code(HttpResponse::k2000k);
        resp->set_status_message("ok");
        resp->set_content_type("text/html");

        resp->AddHeader("Server", "Saber");
        resp->set_body("<html><head><title>This is title</title></head>"
        "<body><h1>Hello</h1>"
        "</body></html>");
    } else if (req.path() == "/hello") {
        resp->set_status_code(HttpResponse::k2000k);
        resp->set_status_message("ok");
        resp->set_content_type("text/plain");
        resp->set_body("hello, world.\n");
    } else {
        resp->set_status_code(HttpResponse::k404NotFound);
        resp->set_status_message("Not Found");
        resp->set_close_connection(true);
    }
}

int main()
{
    //test
    EventLoop loop;
    InetAddress inet_address(8090);
    HttpServer server(&loop, inet_address);
    server.set_httpCallback(OnRequest);
    server.Start();

    loop.Loop();
}
