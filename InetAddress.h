#pragma once

#include "noncopyable.h"

#include <netinet/in.h>
#include <string>


class InetAddress : private noncopyable
{
public:
    explicit InetAddress(const int port = 0);
    InetAddress(const std::string& ip, const int port = 0);
    InetAddress(const InetAddress& addr);
    ~InetAddress();

    void set_sockaddr(struct sockaddr_in& sockaddr);

    const struct sockaddr_in* getSockAddr() const { return &sockaddr_; }
    struct sockaddr_in* getSockAddr() { return &sockaddr_; }

    std::string toIpString();
    std::string toPortString();

private:
    struct sockaddr_in sockaddr_;
};
