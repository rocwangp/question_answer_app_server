#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string>

class InetAddress;

namespace sockets
{

int createNonBlockSocket(sa_family_t family);
int createBlockSocket(sa_family_t family);

void setSocketNonBlock(int sockfd);
void setSocketBlock(int sockfd);

void setReuseAddr(int sockfd);
void setReusePort(int sockfd);
void setCloseOnExec(int sockfd);

void bindAddress(int sockfd, const struct sockaddr_in* sockaddr);
void listen(int sockfd);

int accept(int sockfd, struct sockaddr_in* sockaddr);


void getPeerAddress(int sockfd, InetAddress& peerAddress);
void getLocalAddress(int sockfd, InetAddress& localAddress);

std::string toIpString(const struct sockaddr_in* sockaddr);
std::string toPortString(const struct sockaddr_in* sockaddr);

int send(int sockfd, const char* msg, int len);

void shutdownWrite(int sockfd);
}
