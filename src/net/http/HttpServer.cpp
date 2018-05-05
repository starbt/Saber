#include "HttpServer.h"

#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../Buffer.h"

#include <string>

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop,
           InetAddress& listenAddr
           )
  : server_(loop, listenAddr),
    httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this,std::placeholders::_1,std::placeholders::_2));

}

HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    server_.start();
}


void HttpServer::onConnection(const std::shared_ptr<TcpConnection>& conn)
{
    if (conn->connected())
    {
        auto contextPtr = std::make_shared<HttpContext>();
        conn->setContext(contextPtr);
    }
}

void HttpServer::onMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf)
{
    auto context = conn->getContext();

    if (!context->parseRequst(buf)) {

        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& req)
{
    std::string connection = req.getHeader("Connection");
    bool close = connection == "close" || 
         (req.getVersion() == HttpRequest::kHttp10 && connection != "keep-ALive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.closeConnection())
    {
        conn->shutdown();
    }
}