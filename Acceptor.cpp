#include "Acceptor.h"
#include "Channel.h"
#include "Socket.h"
#include "SocketsOps.h"
#include "Logging.h"

#include <cerrno>

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr) 
    : loop_(loop),
      inetAddress_(addr),
      acceptSocket_(new Socket(sockets::createNonBlockSocket(AF_INET))),
      acceptChannel_(std::make_shared<Channel>(loop, acceptSocket_->fd())),
      idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC))
{
    acceptSocket_->setReuseAddr();
    acceptSocket_->setReusePort();
    acceptSocket_->setCloseOnExec();
    acceptChannel_->setReadCallBack(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_->disableAll();
}

void Acceptor::start()
{
    LOG_DEBUG << "acceptor's fd is " << acceptSocket_->fd();

    acceptSocket_->bindAddress(inetAddress_);
    acceptSocket_->listen();
    acceptChannel_->enableReading();
}
void Acceptor::handleRead()
{
    LOG_DEBUG << "acceptor fd " << acceptSocket_->fd() << " is readable";
    InetAddress peekaddr;
    int sockfd = acceptSocket_->accept(&peekaddr); 
    if(sockfd < 0)
    {
        if(errno == EMFILE)
        {
            LOG_ERROR << "file descriptor use up";
            ::close(idleFd_);
            sockfd = acceptSocket_->accept(&peekaddr);
            ::close(sockfd);
            ::open("dev/null", O_RDONLY);
        }
        else
        {
            LOG_ERROR << "accept error";
        }
    }
    LOG_DEBUG << "new connection fd is " << sockfd;
    newConnectionCallBack_(sockfd, peekaddr); 
}
