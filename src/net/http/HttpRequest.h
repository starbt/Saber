#ifndef SABER_NET_HTTP_HTTPREQUEST_H
#define SABER_NET_HTTP_HTTPREQUEST_H

#include <string>
#include <map>
#include "../../base/Timestamp.h"

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

     bool setVersion(Version v) { version_ = v; }
     Version getVersion () const { return version_; }

     bool setMethod(const char* start, const char* end)
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

     Methord getMethod() const { return methord_; }

     const char* methodString() const
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

     void setPath(const char* start, const char* end) 
     {
         path_.assign(start, end);
     }
     const std::string& getPath() const { return path_; }

     void setQuery(const char* start, const char* end) 
     {
         query_.assign(start, end);
     }
     const std::string& getQuery() const { return query_; }

     void addHeader(const char* start, const char* colon, const char* end) {
         std::string field(start, colon);
         ++colon;
         std::string value(colon, end);
         while (!value.empty() && isspace(value[value.size() - 1]))
         {
             value.resize(value.size() - 1);
         }
         headers_[field] = value;
     }

     std::string getHeader(const std::string& field) const 
     {
         std::string result;
         auto it = headers_.find(field);
         if (it != headers_.end()) {
             result = it->second;
         }
         return result;
     }

    const std::map<std::string, std::string> headers() const { return headers_; }

    void swap(HttpRequest& that) 
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