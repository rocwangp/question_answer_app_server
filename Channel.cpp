#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"

#include <sys/epoll.h>

#include <iostream>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : fd_(fd),
      loop_(loop),
      tied_(false),
      state_(-1)
{

}

Channel::~Channel()
{

}

void Channel::tie(const std::shared_ptr<TcpConnection>& tcpConnection)
{
    tie_ = tcpConnection;
    tied_ = true;
}

void Channel::handleEvent()
{
    LOG_TRACE << "start handle event callback";
    std::shared_ptr<TcpConnection> guard;
    if(tied_)
    {
        guard = tie_.lock();
        if(guard)
        {
            handleEventWithGuard();
        }
    }
    else
    {
        handleEventWithGuard();
    }
    
}

void Channel::handleEventWithGuard()
{
    if(revents_ & EPOLLHUP || !(revents_ & EPOLLIN))
    {
        LOG_TRACE << "events is EPOLLHUP, closeCallBack_";
        closeCallBack_();
    }
    else if(revents_ & EPOLLIN)
    {
        LOG_TRACE << "events is EPOLLIN, readCallBack_";
        readCallBack_();
    }
    else if(revents_ & EPOLLOUT)
    {
        LOG_TRACE << "events is EPOLLOUT, writeCallBack_";
        writeCallBack_();
    }
}


void Channel::update()
{
    loop_->updateChannel(this);
}


