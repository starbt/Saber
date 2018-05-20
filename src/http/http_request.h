#ifndef SABER_HTTP_HTTPREQUEST_H
#define SABER_HTTP_HTTPREQUEST_H

#include <string>
#include <map>
#include "core/timer_stamp.h"

class HttpRequest 
{
 public:
     enum Methord
     {
         kInvalid, kGet, kPost, kHead, kPut, kDelete
     };
     enum Version 
     {
         kUnknown, kHttp10, kHttp11
     }; 

     HttpRequest()
       : methord_(kInvalid),
         version_(kUnknown)
     {}

     bool set_version(Version v) { version_ = v; }
     Version version () const { return version_; }

     bool set_method(const char* start, const char* end)
     {
         std::string m(start, end);
         if (m == "GET") {
             methord_ = kGet;
         } else if (m == "POST") {
             methord_ = kPost;
         } else if (m == "HEAD") {
             methord_ = kHead;
         } else if (m == "PUT") {
             methord_ = kPut;
         } else if (m == "DELETE") {
             methord_ = kDelete;
         } else {
             methord_ = kInvalid;
         }

         return methord_ != kInvalid;
     }

     Methord method() const { return methord_; }

     const char* method_string() const
     {
         const char* result = "UNKNOW";
         switch(methord_) {
            case kGet:
               result = "GET";
               break;
            case kPost:
               result = "POST";
               break;
            case kHead:
               result = "HEAD";
               break;
            case kPut:
               result = "PUT";
               break;
            case kDelete:
               result = "DELETE";
               break;
            default:
               break;
         }
         return result;
     }

     void set_path(const char* start, const char* end) 
     {
         path_.assign(start, end);
     }
     const std::string& path() const { return path_; }

     void set_query(const char* start, const char* end) 
     {
         query_.assign(start, end);
     }
     const std::string& query() const { return query_; }

     void AddHeader(const char* start, const char* colon, const char* end) {
         std::string field(start, colon);
         ++colon;
         std::string value(colon, end);
         while (!value.empty() && isspace(value[value.size() - 1]))
         {
             value.resize(value.size() - 1);
         }
         headers_[field] = value;
     }

     std::string get_header(const std::string& field) const 
     {
         std::string result;
         auto it = headers_.find(field);
         if (it != headers_.end()) {
             result = it->second;
         }
         return result;
     }

    const std::map<std::string, std::string> headers() const { return headers_; }

    void Swap(HttpRequest& that) 
    {
        std::swap(methord_, that.methord_);
        std::swap(version_, that.version_);
        path_.swap(that.path_);
        query_.swap(that.query_);
        headers_.swap(that.headers_);
    }

private:
    Methord methord_;
    Version version_;
    std::string path_;
    std::string query_; 
    std::map<std::string, std::string> headers_;

};

#endif