#ifndef SABER_NET_HTTP_HTTPSERVER_H
#define SABER_NET_HTTP_HTTPSERVER_H

#include <functional>
#include "../TcpServer.h"

class HttpRequest;
class HttpResponse; 

class HttpServer 
{
public:
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;
    HttpServer(EventLoop* loop,
               InetAddress& listenAddr
               );

    ~HttpServer();

    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void start();

private:
    void onConnection(const std::shared_ptr<TcpConnection>& conn);
    void onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf);
    void onRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& );

    TcpServer server_;
    HttpCallback httpCallback_;
};

#endif