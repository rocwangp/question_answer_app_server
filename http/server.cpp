#include "HttpServer.h"

int main()
{
    EventLoop loop;
    InetAddress localaddr(80);
    HttpServer server(&loop, localaddr);
    server.start();
    loop.loop();
    return 0;
}

