#include "http_server.h"

#include "http_context.h"
#include "http_request.h"
#include "http_response.h"
#include "net/buffer.h"
#include <string>

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
    resp->set_status_code(HttpResponse::k404NotFound);
    resp->set_status_message("Not Found");
    resp->set_close_connection(true);
}

HttpServer::HttpServer(EventLoop* loop,
           InetAddress& listen_addr
           )
  : server_(loop, listen_addr),
    httpCallback_(defaultHttpCallback)
{
    server_.set_connectionCallback(std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
    server_.set_messageCallback(std::bind(&HttpServer::OnMessage, this,std::placeholders::_1,std::placeholders::_2));
}

HttpServer::~HttpServer()
{
}

void HttpServer::Start()
{
    server_.Start();
}


void HttpServer::OnConnection(const std::shared_ptr<TcpConnection>& conn)
{
    if (conn->Connected())
    {
        auto contextPtr = std::make_shared<HttpContext>();
        conn->set_context_ptr(contextPtr);
    }
}

void HttpServer::OnMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf)
{
    auto context = conn->context_ptr();

    if (!context->ParseRequst(buf)) {

        conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->Shutdown();
    }

    if (context->GotAll()) {
        OnRequest(conn, context->request());
        context->Reset();
    }
}

void HttpServer::OnRequest(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& req)
{
    std::string connection = req.get_header("Connection");
    bool close = connection == "close" || 
         (req.version() == HttpRequest::kHttp10 && connection != "keep-ALive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.AppendToBuffer(&buf);
    conn->Send(&buf);
    if (response.close_connection())
    {
        conn->Shutdown();
    }
}