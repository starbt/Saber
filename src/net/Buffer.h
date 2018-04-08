#ifndef SABER_BUFFER_H
#define SABER_BUFFER_H

#include <vector>

class Buffer
{
public:
    static const size_t kCheapPrepend;
    static const size_t kInitialSize;

public:
    Buffer(size_t initialSize = kInitialSize)
      : buffer_(kInitialSize),
        readerIndex_(kCheapPrepend),
        writeIndex_(kCheapPrepend) 
{ }

    ssize_t readFd(int fd, int *savedErrno);

public:
    size_t readableBytes() 
    { return writeIndex_ - readerIndex_; }

    size_t writableBytes() 
    { return buffer_.size() - writeIndex_; }

    size_t prependableBytes()
    { return readerIndex_; }

    void append(char* data, size_t len);

private:
    char* begin() 
    {
        return &(*buffer_.begin());
    } 

    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;
};

#endif