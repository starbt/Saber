
#ifndef SABER_INETADDRESS_H
#define SABER_INETADDRESS_H

#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    explicit InetAddress(uint16_t port = 8388);
    InetAddress(std::string &ip, uint16_t port);
    explicit InetAddress(const struct sockaddr_in &addr)
      : addr_(addr)
    { }

    const struct sockaddr_in* GetSockAddr() const { return &addr_; }
    void set_addr(const struct sockaddr_in &addr) { addr_ = addr; }
    std::string ToIpPort();

private:
    struct sockaddr_in addr_;
};


#endif //SABER_INETADDRESS_H