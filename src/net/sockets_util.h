#ifndef SABER_SOCKETSOPS_H
#define SABER_SOCKETSOPS_H

#include <arpa/inet.h>

namespace sockets
{

int CreateNonblockingOrDie(sa_family_t family);
int Connect(int sockfd, const struct sockaddr_in *addr);
void BindOrDie(int sockfd, const struct sockaddr_in *addr);
void ListenOrDie(int sockfd);
int Accept(int sockfd, struct sockaddr_in *addr);


ssize_t Read(int sockfd, void *buf, size_t count);
ssize_t Write(int sockfd, const void *buf, size_t count);
ssize_t Readv(int sockfd, const struct iovec *iov, int iovcnt);

void ToIpPort(char* buf, size_t size, const struct sockaddr_in* addr);
void ToIp(char* buf, size_t size, const struct sockaddr_in* addr);
struct sockaddr_in GetLocalAddr(int sockfd);

void Close(int sockfd);
void ShutdownWrite(int sockfd);

//转换函数
const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
const struct sockaddr_in *sockaddr_in_case(const struct sockaddr *addr);

}



#endif //SABER_SOCKETSOPS_H
