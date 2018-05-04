#ifndef SABER_SOCKETSOPS_H
#define SABER_SOCKETSOPS_H

#include <arpa/inet.h>

namespace sockets
{

int createNonblockingOrDie(sa_family_t family);
int connect(int sockfd, const struct sockaddr_in *addr);
void bindOrDie(int sockfd, const struct sockaddr_in *addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in *addr);


ssize_t read(int sockfd, void *buf, size_t count);
ssize_t write(int sockfd, const void *buf, size_t count);
ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

void toIpPort(char* buf, size_t size, const struct sockaddr_in* addr);
void toIp(char* buf, size_t size, const struct sockaddr_in* addr);
struct sockaddr_in getLocalAddr(int sockfd);

void close(int sockfd);
void shutdownWrite(int sockfd);

//转换函数
const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
const struct sockaddr_in *sockaddr_in_case(const struct sockaddr *addr);

}



#endif //SABER_SOCKETSOPS_H
