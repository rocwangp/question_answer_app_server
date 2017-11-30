#pragma once

#include "./noncopyable.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Timer.h"
#include "CallBack.h"

#include <set>
class TimerQueue : noncopyable
{
public:
    TimerQueue(EventLoop *loop);
    ~TimerQueue();

    
    void addTimer(Timestamp when, TimerCallBack callBack,  double interval);

private:
    typedef std::shared_ptr<Timer> TimerPtr;
    typedef std::pair<Timestamp, TimerPtr> Entry;
    typedef std::set<Entry> TimerSet;
    typedef std::vector<Entry> TimerList;

    void handleRead();
    void addTimerInLoop(const TimerPtr& timer);
    bool insertTimer(const TimerPtr& timer);

    void resetTimerfd(Timestamp when);
    void reset(Timestamp when);
private:

    EventLoop *loop_;
    int timerfd_;
    Channel timerfdChannel_;
    TimerSet timers_;
    TimerList activeTimers_;
};
