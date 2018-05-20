#ifndef SABER_HTTP_HTTPSERVER_H
#define SABER_HTTP_HTTPSERVER_H

#include <functional>
#include "net/tcp_server.h"

class HttpRequest;
class HttpResponse; 

class HttpServer 
{
public:
    typedef std::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;
    HttpServer(EventLoop* loop,
               InetAddress& listen_addr
               );

    ~HttpServer();

    void set_httpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void Start();

private:
    void OnConnection(const std::shared_ptr<TcpConnection>& conn);
    void OnMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf);
    void OnRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& );

    TcpServer server_;
    HttpCallback httpCallback_;
};

#endif