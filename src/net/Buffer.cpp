#include "Buffer.h"
#include <sys/uio.h>
#include <error.h>

#include "SocketsOps.h" 

const size_t Buffer::kCheapPrepend = 8;
const size_t Buffer::kInitialSize = 1024;

ssize_t Buffer::readFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable)
    {
        writerIndex_ += n;
    } 
    else {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }
    return n;
}

void Buffer::append(const char* data, size_t len)
{
    if (writableBytes() < len) 
    {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else 
        {
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readableBytes();
        }
    }
    std::copy(data, data + len, begin() + writerIndex_);
    writerIndex_ += len;
}