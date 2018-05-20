#ifndef SABER_HTTP_HTTPRESPONSE_H
#define SABER_HTTP_HTTPRESPONSE_H

#include <map>
#include <string> 

class Buffer;

class HttpResponse
{
public:
    enum HttpStatusCode
    {
        kUnknown,
        k2000k = 200,
        k301MovePermanently = 301,
        k400BadRequest = 400,
        k404NotFound = 404,
    };
   explicit HttpResponse(bool close)
     : status_code_(kUnknown),
       close_connection_(close)
     {
     }

    void set_status_code(HttpStatusCode code) { status_code_ = code; }
    void set_status_message(const std::string& message) { status_message_= message; }
    void set_close_connection(bool on) { close_connection_ = on; }

    bool close_connection() const 
    { return close_connection_; }

    void set_content_type(const std::string& content_type)
    {
        AddHeader("Content-Type", content_type);
    }

    void AddHeader(const std::string& key, const std::string& value)
    {
        headers_[key] = value;
    }

    void set_body(const std::string& body)
    {
        body_ = body;
    }

    void AppendToBuffer(Buffer* output);

private:
    std::map<std::string, std::string> headers_;
    HttpStatusCode status_code_;
    std::string status_message_;
    bool close_connection_;
    std::string body_;
};

#endif