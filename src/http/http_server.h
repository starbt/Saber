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
    typedef std::function<void ()> TimerCallback;

    HttpServer(EventLoop* loop,
               InetAddress& listen_addr
               );

    ~HttpServer();

    void set_httpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void set_thread_num(int num_threads);

    void Start();

    //一组定时回调函数
    void RunAt(const Timestamp &time, const TimerCallback &cb);
    void RunAfter(double delay, const TimerCallback &cb);
    void RunEvery(double interval, const TimerCallback &cb);

private:
    void OnConnection(const std::shared_ptr<TcpConnection>& conn);
    void OnMessage(const std::shared_ptr<TcpConnection>& conn, Buffer* buf);

    void ResponseMsg(const std::shared_ptr<TcpConnection>& conn, const HttpRequest& );

    TcpServer server_;
    HttpCallback httpCallback_;
};

#endif