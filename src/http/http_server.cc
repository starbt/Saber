#include "http_server.h"
#include<pthread.h>
#include <string>
#include "http_context.h"
#include "http_request.h"
#include "http_response.h"
#include "net/buffer.h"
#include "core/event_loop.h"

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


void HttpServer::set_thread_num(int num_threads)
{
    server_.set_thread_num(num_threads);
}

void HttpServer::Start()
{
    server_.Start();
}


void HttpServer::RunAt(const Timestamp &time, const TimerCallback &cb)
{
    server_.RunAt(time, cb);
}
void HttpServer::RunAfter(double delay, const TimerCallback &cb)
{
    server_.RunAfter(delay, cb);

}
void HttpServer::RunEvery(double interval, const TimerCallback &cb)
{
    server_.RunEvery(interval, cb);
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
        ResponseMsg(conn, context->request());
        context->Reset();
    }
}

void HttpServer::ResponseMsg(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& req)
{
    std::string connection = req.get_header("Connection");
    bool close = connection == "close" || 
         (req.version() == HttpRequest::kHttp10 && connection != "keep-ALive");
    HttpResponse resp(close);

    printf("thread loop tid:%d\n",pthread_self());

    //std::cout << "Headers " << req.method_string() <<  " " << req.path() << std::endl;
    //auto headers = req.headers();
    //for (auto it = headers.begin(); it != headers.end(); ++it)
    //{
    //    std::cout << it->first << ":" << it->second << std::endl;
    //}

    if (req.path() == "/") {
        resp.set_status_code(HttpResponse::k2000k);
        resp.set_status_message("ok");
        resp.set_content_type("text/html");
        resp.AddHeader("Server", "Saber");
        resp.set_body("<html><head><title>This is title</title></head>"
        "<body><h1>Hello</h1>"
        "</body></html>");
    } else if (req.path() == "/hello") {
        resp.set_status_code(HttpResponse::k2000k);
        resp.set_status_message("ok");
        resp.set_content_type("text/plain");
        resp.set_body("hello, world.\n");
    } else {
        resp.set_status_code(HttpResponse::k404NotFound);
        resp.set_status_message("Not Found");
        resp.set_close_connection(true);
    }
    Buffer buf;
    resp.AppendToBuffer(&buf);
    conn->Send(&buf);
    if (resp.close_connection())
    {
        conn->Shutdown();
    }
}