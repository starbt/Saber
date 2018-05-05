
#include <iostream>

#include "../src/net/http/HttpServer.h"
#include "../src/net/http/HttpResponse.h"
#include "../src/net/http/HttpRequest.h"
#include "../src/net/EventLoop.h"

void onRequest(const HttpRequest& req, HttpResponse* resp)
{
    std::cout << "Headers " << req.methodString() <<  " " << req.getPath() << std::endl;
    auto headers = req.headers();
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    if (req.getPath() == "/") {
        resp->setStatusCode(HttpResponse::k2000k);
        resp->setStatusMessage("ok");
        resp->setContenType("text/html");

        resp->addHeader("Server", "Saber");
        resp->setBody("<html><head><title>This is title</title></head>"
        "<body><h1>Hello</h1>"
        "</body></html>");
    } else if (req.getPath() == "/hello") {
        resp->setStatusCode(HttpResponse::k2000k);
        resp->setStatusMessage("ok");
        resp->setContenType("text/plain");
        resp->setBody("hello, world.\n");
    } else {
        resp->setStatusCode(HttpResponse::k404NotFound);
        resp->setStatusMessage("Not Found");
        resp->setCloseConnection(true);
    }
}

int main()
{
    EventLoop loop;
    InetAddress inetAddress(8090);
    HttpServer server(&loop, inetAddress);
    server.setHttpCallback(onRequest);
    server.start();

    loop.loop();
}