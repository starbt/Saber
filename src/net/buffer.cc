#include "buffer.h"
#include <sys/uio.h>
#include <error.h>

#include "sockets_util.h" 

const char Buffer::kCRLF[] = "\r\n"; 

const size_t Buffer::kCheapPrepend = 8;
const size_t Buffer::kInitialSize = 1024;

ssize_t Buffer::ReadFd(int fd, int *savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = WritableBytes();
    vec[0].iov_base = Begin() + write_index_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = sockets::Readv(fd, vec, iovcnt);
    if (n < 0)
    {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable)
    {
        write_index_ += n;
    } 
    else {
        write_index_ = buffer_.size();
        Append(extrabuf, n - writable);
    }
    return n;
}

void Buffer::Append(const char* data, size_t len)
{
    if (WritableBytes() < len) 
    {
        if (WritableBytes() + PrependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(write_index_ + len);
        }
        else 
        {
            std::copy(Begin() + read_index_,
                      Begin() + write_index_,
                      Begin() + kCheapPrepend);
            read_index_ = kCheapPrepend;
            write_index_ = read_index_ + ReadableBytes();
        }
    }
    std::copy(data, data + len, Begin() + write_index_);
    write_index_ += len;
}