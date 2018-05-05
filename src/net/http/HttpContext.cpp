#include "HttpContext.h"
#include "../Buffer.h"
#include "../../base/Timestamp.h"

#include<algorithm>

bool HttpContext::processRequestLine(const char*begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && request_.setMethod(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end) 
        {
            const char* question = std::find(start, space, '?');
            if (question != space) 
            {
                request_.setPath(start, question);
                request_.setQuery(question, space);
            } else{
                request_.setPath(start, space);
            }
            start = space + 1;
            succeed = (end - start == 8 && std::equal(start, end - 1, "HTTP/1."));
            if (succeed) 
            {
                if (*(end - 1) == '1') 
                {
                    request_.setVersion(HttpRequest::kHttp11);
                } else if ( *(end - 1) == '0') {
                    request_.setVersion(HttpRequest::kHttp10);
                } else {
                    succeed = false;
                }
            }
        }
    }
    printf("request method:%s,\n",request_.methodString());
    printf("request path:%s,\n",request_.getPath().c_str());
    printf("request query:%s,\n",request_.getQuery().c_str());
    printf("request version:%d,\n",request_.getVersion());
    return succeed;
}


bool HttpContext::parseRequst(Buffer* buf)
{
    bool ok = true;
    bool hasMore = true;
    while (hasMore)
    {
        if (state_ == kExpectRequestLine)
        {
            const char* crlf = buf->findCRLF();
            if (crlf) 
            {
                ok = processRequestLine(buf->peek(), crlf);
                if (ok) 
                {
                    buf->retrieveUntil(crlf + 2);
                    state_ = kExpectHeaders; 
                } else {
                    hasMore = false;
                }
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectHeaders) {
            const char* crlf = buf->findCRLF();
            if (crlf) 
            {
                const char* colon = std::find(buf->peek(), crlf, ':');
                if (colon != crlf) 
                {
                    request_.addHeader(buf->peek(), colon, crlf);
                } else {
                    state_ = kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            } else {
                hasMore = false;
            }
        } else if (state_ == kExpectBody) {
            // add later
        }
    }
    return ok;
}