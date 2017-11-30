#pragma once

#include "noncopyable.h"
#include "Timestamp.h"
#include "CallBack.h"

#include <memory>
#include <vector>
#include <atomic>

class Poller;
class Channel;
class MutexLock;
class TimerQueue;

class EventLoop : private noncopyable
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();
    
    void loop();
    void quit();
    void updateChannel(Channel* channel);

    void runInLoop(Functor cb);
    void queueInLoop(Functor cb);

    void runAt(Timestamp when, TimerCallBack callBack, double interval);
    void runEvery(TimerCallBack callBack, double interval);
private:
    void doPendingFunctors();
    void wakeup();

    void handleRead();
    void handleClose();
private:
    typedef std::vector<Channel*> ChannelList;
    typedef std::vector<Functor> FunctorList;

    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;
    std::atomic<bool> quit_;

    std::unique_ptr<MutexLock> mutex_; 
    FunctorList pendingFunctors_;

    int tid_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;

    std::atomic<bool> polling_;

    std::unique_ptr<TimerQueue> timerQueue_;
};
