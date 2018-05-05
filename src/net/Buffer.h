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
        readerIndex_(kCheapPrepend),
        writerIndex_(kCheapPrepend) 
   { }

    ssize_t readFd(int fd, int *savedErrno);

public:
    size_t readableBytes() 
    { return writerIndex_ - readerIndex_; }

    size_t writableBytes() 
    { return buffer_.size() - writerIndex_; }

    size_t prependableBytes()
    { return readerIndex_; }

    void append(const char* data, size_t len);
    
    void append(const std::string& str) 
    {
        append(str.c_str(), str.size());
    }

    const char* peek() const 
    { return begin() + readerIndex_; }
    

    std::string retrieveAllAsString() 
    {
        int len = readableBytes();
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void retrieve(size_t len) 
    {
        if (len < readableBytes()) 
        {
            readerIndex_ += len;
        }
        else
        {
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend;
        }
    } 

    void retrieveUntil(const char* end)
    {
        retrieve(end - peek());
    }

    void retrievePrepend() 
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    char* beginWrite() 
    {
        return begin() + writerIndex_;
    }

    const char* beginWrite() const
    {
        return begin() + writerIndex_;
    }

public:
    const char* findCRLF() const 
    {
       const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2); 
       return crlf == beginWrite() ? NULL : crlf;
    }
    

private:
    char* begin() 
    {
        return &(*buffer_.begin());
    } 

    const char* begin() const 
    {
        return &(*buffer_.begin());
    }

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

    static const char kCRLF[];
};

#endif