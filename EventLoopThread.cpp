#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Thread.h"
#include "Condition.h"
#include "MutexLockGuard.h"
#include "MutexLock.h"
#include <iostream>


EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      thread_(new Thread()),
      mutex_(new MutexLock()),
      cond_(new Condition(mutex_->mutex()))

{
    thread_->setThreadCallBack(std::bind(&EventLoopThread::threadFunc, this));
}


EventLoopThread::~EventLoopThread()
{

}

EventLoop* EventLoopThread::startLoop()
{
    thread_->start();
    MutexLockGuard guard(mutex_->mutex());
    while(!loop_)
    {
        cond_->wait();
    }

    return loop_;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        MutexLockGuard guard(mutex_->mutex());
        if(!loop_)
        {
            loop_ = &loop;
            cond_->notify();
        }
    }
    loop_->loop();
    loop_ = nullptr;
    pthread_exit(NULL);
}
