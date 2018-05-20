#include "http_response.h"
#include "net/buffer.h"
#include <stdio.h>

void HttpResponse::AppendToBuffer(Buffer* output)
{
    char buf[32];
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", status_code_);
    output->Append(buf);
    output->Append(status_message_); 
    output->Append("\r\n");

    if (close_connection_)
    {
        output->Append("connection: close\r\n");
    } 
    else 
    {
        snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
        output->Append(buf);
        output->Append("Connection: Keep-Alive\r\n");
    }
    for (auto it = headers_.begin(); it != headers_.end(); ++it)
    {
        output->Append(it->first);
        output->Append(": ");
        output->Append(it->second);
        output->Append("\r\n");
    }
    output->Append("\r\n");
    output->Append(body_);
}


