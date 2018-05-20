#ifndef SABER_BUFFER_H
#define SABER_BUFFER_H

#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

class Buffer
{
public:
    static const size_t kCheapPrepend;
    static const size_t kInitialSize;

public:
    Buffer(size_t initialSize = kInitialSize)
      : buffer_(kInitialSize),
        read_index_(kCheapPrepend),
        write_index_(kCheapPrepend) 
   { }

    ssize_t ReadFd(int fd, int *savedErrno);

public:
    size_t ReadableBytes() 
    { return write_index_ - read_index_; }

    size_t WritableBytes() 
    { return buffer_.size() - write_index_; }

    size_t PrependableBytes()
    { return read_index_; }

    void Append(const char* data, size_t len);
    
    void Append(const std::string& str) 
    {
        Append(str.c_str(), str.size());
    }

    const char* Peek() const 
    { return Begin() + read_index_; }
    

    std::string RetrieveAllAsString() 
    {
        int len = ReadableBytes();
        std::string result(Peek(), len);
        Retrieve(len);
        return result;
    }

    void Retrieve(size_t len) 
    {
        if (len < ReadableBytes()) 
        {
            read_index_ += len;
        }
        else
        {
            read_index_ = kCheapPrepend;
            write_index_ = kCheapPrepend;
        }
    } 

    void RetrieveUntil(const char* end)
    {
        Retrieve(end - Peek());
    }

    void RetrievePrepend() 
    {
        read_index_ = kCheapPrepend;
        write_index_ = kCheapPrepend;
    }

    char* BeginWrite() 
    {
        return Begin() + write_index_;
    }

    const char* BeginWrite() const
    {
        return Begin() + write_index_;
    }

public:
    const char* FindCRLF() const 
    {
       const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2); 
       return crlf == BeginWrite() ? NULL : crlf;
    }
    

private:
    char* Begin() 
    {
        return &(*buffer_.begin());
    } 

    const char* Begin() const 
    {
        return &(*buffer_.begin());
    }

    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;

    static const char kCRLF[];
};

#endif