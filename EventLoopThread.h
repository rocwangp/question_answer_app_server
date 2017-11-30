#pragma once

#include "noncopyable.h"

#include <memory>

class EventLoop;
class Thread;
class Condition;
class MutexLock;

class EventLoopThread : private noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* startLoop();
private:
    void threadFunc();
private:
    EventLoop *loop_;
    
    std::unique_ptr<Thread> thread_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<Condition> cond_;
};
