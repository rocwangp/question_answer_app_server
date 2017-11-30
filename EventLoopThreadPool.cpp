#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "Logging.h"

#include <iostream>

const int EventLoopThreadPool::kNumEventLoopThreads = 100;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop)
    : loop_(loop),
      next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}


void EventLoopThreadPool::start(int numThreads)
{
    for(int i = 0; i < numThreads; ++i)
    {
        eventLoopThreads_.emplace_back(std::make_shared<EventLoopThread>());
        eventLoops_.push_back(eventLoopThreads_.back()->startLoop());
    }

    LOG_INFO << "create " << numThreads << " EventLoopThreads Done";
}


EventLoop* EventLoopThreadPool::nextLoop()
{
    if(eventLoops_.empty())
    {
        return loop_;
    }
    else
    {
        EventLoop* loop = eventLoops_[next_];
        if(++next_ >= static_cast<int>(eventLoops_.size()))
        {
            next_ = 0;
        }
        return loop;
    }
}
