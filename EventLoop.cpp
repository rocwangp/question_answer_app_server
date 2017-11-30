#include "EventLoop.h"
#include "EPollPoller.h"
#include "Channel.h"
#include "MutexLock.h"
#include "MutexLockGuard.h"
#include "CurrentThread.h"
#include "TimerQueue.h"
#include "Logging.h"

#include <sys/eventfd.h>

EventLoop::EventLoop()
   : poller_(new EPollPoller()),
     quit_(false),
     mutex_(new MutexLock()),
     tid_(CurrentThread::tid()),
     wakeupFd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
     wakeupChannel_(new Channel(this, wakeupFd_)),
     polling_(false),
     timerQueue_(new TimerQueue(this))
{
   wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead, this));
   wakeupChannel_->setCloseCallBack(std::bind(&EventLoop::handleClose, this));
   wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{

}

void EventLoop::loop()
{
    while(!quit_)
    {
        activeChannels_.clear();
        polling_ = true;
        poller_->poll(activeChannels_);
        polling_ = false;
        for(Channel* channel : activeChannels_)
        {
            channel->handleEvent();
        }
        doPendingFunctors();
    }
}

void EventLoop::quit()
{
    quit_ = true;
    wakeup();
}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}


void EventLoop::runInLoop(Functor cb)
{
    if(CurrentThread::tid() == tid_)
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    MutexLockGuard guard(mutex_->mutex());
    pendingFunctors_.emplace_back(std::move(cb));
    if(polling_)
        wakeup();
}

void EventLoop::doPendingFunctors()
{
    MutexLockGuard guard(mutex_->mutex());
    for(auto cb : pendingFunctors_)
    {
        cb();
    }
    pendingFunctors_.clear();
}


void EventLoop::wakeup()
{
    LOG_DEBUG << "wakeup EventLoopThread " << tid_;
    uint64_t optval = 1;
    ::write(wakeupFd_, &optval, sizeof(optval));
}
void EventLoop::handleRead()
{
    uint64_t one = 1;
    ::read(wakeupFd_, &one, sizeof(one));
}

void EventLoop::handleClose()
{
    wakeupChannel_->disableAll();
    ::close(wakeupFd_);
}

void EventLoop::runAt(Timestamp when, TimerCallBack callBack, double interval)
{
   LOG_DEBUG << "in runAt";
   timerQueue_->addTimer(when, callBack, interval); 
}


void EventLoop::runEvery(TimerCallBack callBack, double interval)
{
    Timestamp when(Timestamp::now());
    when.resetTime(when.microSecondsSinceEpoch() + interval);
    runAt(when, callBack, interval);
}
