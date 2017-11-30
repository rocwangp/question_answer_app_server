#include "Socket.h"
#include "SocketsOps.h"
#include "InetAddress.h"

#include <unistd.h>

Socket::Socket(int sockfd)
    : sockfd_(sockfd)
{

}

Socket::~Socket()
{
    ::close(sockfd_);
}


void Socket::bindAddress(const InetAddress& localaddr)
{
    sockets::bindAddress(sockfd_, localaddr.getSockAddr());
}

void Socket::listen()
{
    sockets::listen(sockfd_);
}

int Socket::accept(InetAddress *localaddr)
{
    return sockets::accept(sockfd_, localaddr->getSockAddr());
}

void Socket::setReuseAddr()
{
    sockets::setReuseAddr(sockfd_);
}

void Socket::setReusePort()
{
    sockets::setReusePort(sockfd_);
}

void Socket::setCloseOnExec()
{
    sockets::setCloseOnExec(sockfd_);
}
