#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <string.h>
int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sockaddr;
    bzero(&sockaddr, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(9000);
    inet_pton(AF_INET, "127.0.0.1", &sockaddr.sin_addr);
    connect(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr));

    
}
