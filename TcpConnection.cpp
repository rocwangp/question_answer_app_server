#include "TcpConnection.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Socket.h"
#include "SocketsOps.h"
#include "Logging.h"

class Timestamp;
TcpConnection::TcpConnection(EventLoop* loop, int sockfd, std::string name)
    : loop_(loop),
      name_(name),
      channel_(std::make_shared<Channel>(loop, sockfd)),
      socket_(new Socket(sockfd))
{
    channel_->setReadCallBack(std::bind(&TcpConnection::handleRead, this));
    channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
    channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
}


TcpConnection::~TcpConnection()
{
    LOG_TRACE << "TcpConnection Destroyed";
}


void TcpConnection::connectEstablish()
{
    LOG_TRACE << "in TcpConnection::connectEstablish";
    channel_->tie(shared_from_this());
    channel_->enableReading();
}

void TcpConnection::connectDestroy()
{
}

void TcpConnection::shutdownWrite()
{
    LOG_DEBUG << "shutdown " << channel_->fd() << " write";
    sockets::shutdownWrite(channel_->fd());
}

void TcpConnection::handleRead()
{
    LOG_DEBUG << channel_->fd() << " is readable";
    int n = inputBuffer_.readFd(channel_->fd());
    if(n > 0)
    {
        LOG_DEBUG << "read " << n << " bytes from client";
        messageCallBack_(shared_from_this(), inputBuffer_);
    }
    else if(n == 0)
    {
        LOG_DEBUG << "read 0 bytes, close connection";
        handleClose();
    }
    else
    {
        LOG_ERROR << "read error";
    }
}

void TcpConnection::handleClose()
{
    LOG_DEBUG << channel_->fd() << " is closed";
    channel_->disableAll();
    TcpConnectionPtr guard(shared_from_this());
    closeCallBack_(guard);
}

void TcpConnection::handleWrite()
{
    int n = sockets::send(channel_->fd(), 
                          outputBuffer_.peek(),
                          outputBuffer_.writableBytes());
    if(n < 0)
    {
        LOG_ERROR << "sockets::send error";
    }
    else if(n == outputBuffer_.writableBytes())
    {
        LOG_DEBUG << "send " << n << " bytes to client, buffer is empty";
        channel_->disableWriting();
        outputBuffer_.retrieve(n);
    }
    else
    {
        LOG_DEBUG << "send " << n << " bytes to client";
        outputBuffer_.retrieve(n);
    }

}

void TcpConnection::send(const std::string& msg)
{
   int ret = sockets::send(channel_->fd(), msg.c_str(), msg.size()); 
   if(ret < static_cast<int>(msg.size()))
   {
       int remainer = 0;
       if(ret > 0)
           remainer = ret;
       outputBuffer_.append(msg.c_str() + remainer, msg.size() - remainer);
       channel_->enableWriting(); 
       LOG_DEBUG << "send " << ret << " bytes to client, put the last to buffer";
   }
   else
   {
       LOG_DEBUG << "send done";
   }
}
