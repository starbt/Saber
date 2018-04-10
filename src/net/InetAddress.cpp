//
// Created by xcv on 17-12-13.
//

#include <memory.h>
#include <string>
#include <arpa/inet.h>
#include "InetAddress.h"
#include "SocketsOps.h"

using namespace saber;

InetAddress::InetAddress(uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_addr.s_addr = htobe32(INADDR_ANY);
    addr_.sin_port = htobe16(port);
}

InetAddress::InetAddress(std::string &ip, uint16_t port) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0 ) {
        printf("inet_pton error.\n");
    }
    addr_.sin_port = htobe16(port);
}

std::string InetAddress::toIpPort() 
{ 
    char buf[64] = "";
    sockets::toIpPort(buf, sizeof buf, addr_);
    return buf;
}