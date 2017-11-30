#include "InetAddress.h"
#include "SocketsOps.h"

#include <cstring>

InetAddress::InetAddress(const int port)
{
    sockaddr_.sin_family = AF_INET;
    sockaddr_.sin_port = htons(port);
    sockaddr_.sin_addr.s_addr = INADDR_ANY;

}
InetAddress::InetAddress(const std::string& ip, const int port)
{
    sockaddr_.sin_family = AF_INET;      
    sockaddr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &sockaddr_.sin_addr);
}

InetAddress::InetAddress(const InetAddress& addr)
{
   memcpy(&sockaddr_, &addr.sockaddr_, sizeof(sockaddr_)); 
}

InetAddress::~InetAddress()
{

}


void InetAddress::set_sockaddr(struct sockaddr_in& sockaddr)
{
    sockaddr_ = std::move(sockaddr);
}
std::string InetAddress::toIpString()
{
    return sockets::toIpString(&sockaddr_);
}

std::string InetAddress::toPortString()
{
    return sockets::toPortString(&sockaddr_);
}
