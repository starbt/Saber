#ifndef SABER_HTTP_HTTPCONTEXT_H
#define SABER_HTTP_HTTPCONTEXT_H

#include "http_request.h"

class Buffer;

class HttpContext 
{
public:
    enum HttpRequestParseState 
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
      : state_(kExpectRequestLine) 
    {
    }

    bool ParseRequst(Buffer* buf);

    bool GotAll() const { return state_ == kGotAll; }

    void Reset() 
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.Swap(dummy);
    }

    HttpRequest& request() { return request_; }

private:
    bool ProcessRequestLine(const char*begin, const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};

#endif