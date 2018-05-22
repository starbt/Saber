#include "http_context.h"
#include<algorithm>

#include "net/buffer.h"
#include "time/timer_stamp.h"


bool HttpContext::ProcessRequestLine(const char*begin, const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');
    if (space != end && request_.set_method(start, space))
    {
        start = space + 1;
        space = std::find(start, end, ' ');
        if (space != end) 
        {
            const char* question = std::find(start, space, '?');
            if (question != space) 
            {
                request_.set_path(start, question);
                request_.set_query(question, space);
            } else{
                request_.set_path(start, space);
            }
            start = space + 1;
            succeed = (end - start == 8 && std::equal(start, end - 1, "HTTP/1."));
            if (succeed) 
            {
                if (*(end - 1) == '1') 
                {
                    request_.set_version(HttpRequest::kHttp11);
                } else if ( *(end - 1) == '0') {
                    request_.set_version(HttpRequest::kHttp10);
                } else {
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}


bool HttpContext::ParseRequst(Buffer* buf)
{
    bool ok = true;
    bool has_more = true;
    while (has_more)
    {
        if (state_ == kExpectRequestLine)
        {
            const char* crlf = buf->FindCRLF();
            if (crlf) 
            {
                ok = ProcessRequestLine(buf->Peek(), crlf);
                if (ok) 
                {
                    buf->RetrieveUntil(crlf + 2);
                    state_ = kExpectHeaders; 
                } else {
                    has_more = false;
                }
            } else {
                has_more = false;
            }
        } else if (state_ == kExpectHeaders) {
            const char* crlf = buf->FindCRLF();
            if (crlf) 
            {
                const char* colon = std::find(buf->Peek(), crlf, ':');
                if (colon != crlf) 
                {
                    request_.AddHeader(buf->Peek(), colon, crlf);
                } else {
                    state_ = kGotAll;
                    has_more = false;
                }
                buf->RetrieveUntil(crlf + 2);
            } else {
                has_more = false;
            }
        } else if (state_ == kExpectBody) {
            // add later
        }
    }
    return ok;
}