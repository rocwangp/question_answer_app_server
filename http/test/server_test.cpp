#include <iostream>
#include <string>

using namespace std;

#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
    string headers="POST / HTTP/1.1\r\n\r\n{\"question\":\"如何变得会聊天\",\"typeId\":\"0\"}";

    cout << strlen(headers.c_str()) << endl;
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr;
    ::bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = ::htons(80);
    ::inet_pton(AF_INET, "39.106.114.141", &sockaddr.sin_addr);

    ::connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    ::send(sockfd, headers.c_str(), headers.size(), MSG_NOSIGNAL);
    char buf[4096];
    bzero(buf, sizeof(buf));
    int n = ::recv(sockfd, buf, sizeof(buf), 0);
    buf[n] = '\0';
    cout << buf << endl;
    ::close(sockfd);

    sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    ::bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = ::htons(80);
    ::inet_pton(AF_INET, "39.106.114.141", &sockaddr.sin_addr);
    ::connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    headers="POST / HTTP/1.1\r\n\r\n{\"answerIds\":\"201,202,203\",\"typeId\":\"1\"}";
    ::send(sockfd, headers.c_str(), headers.size(), MSG_NOSIGNAL);
    while(true)
    {
        n = ::recv(sockfd, buf, sizeof(buf), 0);
        if(n <= 0)
            break;
        buf[n] = '\0';
        cout << n<< endl;
        cout << buf << endl;
    }
    
    ::close(sockfd);
    return 0;
}
