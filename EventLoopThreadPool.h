#pragma once

#include "noncopyable.h"

#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : private noncopyable
{
public:
    EventLoopThreadPool(EventLoop* loop);
    ~EventLoopThreadPool();

    void start(int numThreads = kNumEventLoopThreads);
    EventLoop* nextLoop();
private:
    static const int kNumEventLoopThreads;

    typedef std::shared_ptr<EventLoopThread> EventLoopThreadPtr;

    std::vector<EventLoop*> eventLoops_;
    std::vector<EventLoopThreadPtr> eventLoopThreads_;

    EventLoop* loop_;
    int next_;
};
