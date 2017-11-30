#include "SocketsOps.h"
#include "InetAddress.h"
#include "Logging.h"

#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string.h> //bzero
namespace
{

static const int kSocketListenSize = 1000;
}

int sockets::createNonBlockSocket(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM, 0);
    setSocketNonBlock(sockfd);
    return sockfd;
}

int sockets::createBlockSocket(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM, 0);
    return sockfd;
}

void sockets::setSocketNonBlock(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL);
    flags |= O_NONBLOCK;
    ::fcntl(sockfd, F_SETFL, flags);
}

void sockets::setSocketBlock(int sockfd)
{
    int flags = ::fcntl(sockfd, F_GETFL);
    flags &= (~O_NONBLOCK);
    ::fcntl(sockfd, F_SETFL, flags);
}

void sockets::setReuseAddr(int sockfd)
{
    int optval = 1;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void sockets::setReusePort(int sockfd)
{
    int optval = 1;
    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void sockets::setCloseOnExec(int sockfd)
{
    int optval = 1;
    ::setsockopt(sockfd, SOL_SOCKET, SOCK_CLOEXEC, &optval, sizeof(optval));
}

void sockets::bindAddress(int sockfd, const struct sockaddr_in* sockaddr)
{
    int ret = ::bind(sockfd, (struct sockaddr*)sockaddr, static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if(ret < 0)
        LOG_ERROR << "bind error " << strerror(errno);
}

void sockets::listen(int sockfd)
{
    int ret = ::listen(sockfd, kSocketListenSize);
    if(ret < 0)
        LOG_ERROR << "listen error " << strerror(errno);
}

int sockets::accept(int sockfd, struct sockaddr_in* sockaddr)
{
    socklen_t len = sizeof(struct sockaddr_in);
    int fd = ::accept(sockfd, (struct sockaddr*)sockaddr, &len);
    if(fd < 0)
        return fd;
    setSocketNonBlock(fd);
    return fd;
}


void sockets::getPeerAddress(int sockfd, InetAddress& peerAddress)
{
    struct sockaddr_in sockaddr;
    bzero(&sockaddr, sizeof(sockaddr));
    socklen_t len = static_cast<socklen_t>(sizeof(sockaddr));
    ::getpeername(sockfd, (struct sockaddr*)&sockaddr, &len);
    peerAddress.set_sockaddr(sockaddr);
}


void sockets::getLocalAddress(int sockfd, InetAddress& localAddress)
{
    struct sockaddr_in sockaddr;
    ::bzero(&sockaddr, sizeof(sockaddr));
    socklen_t len = static_cast<socklen_t>(sizeof(sockaddr));
    ::getsockname(sockfd, (struct sockaddr*)&sockaddr, &len);
    localAddress.set_sockaddr(sockaddr);
}

std::string sockets::toIpString(const struct sockaddr_in* sockaddr)
{
    char ip[1024];
    bzero(ip, sizeof(ip));

    ::inet_ntop(sockaddr->sin_family, &sockaddr->sin_addr, ip, sizeof(ip));
    return ip;
}


std::string sockets::toPortString(const struct sockaddr_in* sockaddr)
{
    int port = static_cast<int>(::ntohs(sockaddr->sin_port));
    std::stringstream oss;
    oss << port;
    return oss.str();
}

int sockets::send(int sockfd, const char* msg, int len)
{
    return ::send(sockfd, msg, len, MSG_NOSIGNAL);
}

void sockets::shutdownWrite(int sockfd)
{
    ::shutdown(sockfd, SHUT_WR);
}
