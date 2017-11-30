#pragma once

#include "noncopyable.h"

class InetAddress;

class Socket : private noncopyable
{
public:
    Socket(const int sockfd);
    ~Socket();

    void bindAddress(const InetAddress& localaddr);
    void listen();
    int accept(InetAddress* localaddr);

    int fd()  { return sockfd_; }

    void setReuseAddr();
    void setReusePort();
    void setCloseOnExec();
private:
    const int sockfd_;
};
