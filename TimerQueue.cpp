#include "TimerQueue.h"
#include "Logging.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      timerfdChannel_(loop, timerfd_)
{
    timerfdChannel_.setReadCallBack(std::bind(&TimerQueue::handleRead, this));
    timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
    ::close(timerfd_);
}

void TimerQueue::addTimer(Timestamp when, TimerCallBack callBack, double interval)
{
    TimerPtr timer(std::make_shared<Timer>(when, callBack, interval));
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
    LOG_DEBUG << "in addTimer";
}

void TimerQueue::addTimerInLoop(const TimerPtr& timer)
{
    LOG_DEBUG << "in addTimerInLoop";
    bool updateTimerfd = insertTimer(timer);
    if(updateTimerfd)
    {
        resetTimerfd(timer->expiration());
    }
}

bool TimerQueue::insertTimer(const TimerPtr& timer)
{
    bool updateTimerfd = false;
    if(timers_.empty() || (timer->expiration() < timers_.begin()->first)) 
    {
        updateTimerfd = true;
    }

    Timestamp when = timer->expiration();
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    LOG_DEBUG << microseconds / Timestamp::kMicroSecondsPerSecond;

    timers_.insert(Entry(timer->expiration(), timer));
    return updateTimerfd;
}

void TimerQueue::resetTimerfd(Timestamp when)
{
    LOG_DEBUG << "reset timerfd";
    int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
    if(microseconds <= 0)
        microseconds = 0;
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<time_t>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);

    LOG_DEBUG << static_cast<int>(ts.tv_sec);
    struct itimerspec newValue;
    struct itimerspec oldValue;
    bzero(&newValue, sizeof(newValue));
    bzero(&oldValue, sizeof(oldValue));
    newValue.it_value = ts;
    ::timerfd_settime(timerfd_, 0, &newValue, &oldValue);
}

void TimerQueue::handleRead()
{
    LOG_INFO << "in handleRead";
    Timestamp now(Timestamp::now());    
    auto it = timers_.begin();
    while(it != timers_.end() && now >= it->first)
        ++it;
    activeTimers_.assign(timers_.begin(), it);
    timers_.erase(timers_.begin(), it);
    
    LOG_DEBUG << timers_.size();
    
    for(auto &timer : activeTimers_)
    {
        LOG_INFO << "start call back";
        timer.second->run();
    }

    for(auto &timer : activeTimers_)
    {
        if(timer.second->isRepeat())
        {
            timer.second->resetTimer(now);
            timer.first = timer.second->expiration();
            timers_.insert(timer);
        }
    }

    {
        TimerList timerList;
        std::swap(timerList, activeTimers_);
    }

    LOG_DEBUG << timers_.size();
    it = timers_.begin();
    while(it != timers_.end())
    {
        Timestamp when = it->first;
        int64_t microseconds = when.microSecondsSinceEpoch() - Timestamp::now().microSecondsSinceEpoch();
        LOG_DEBUG << microseconds / Timestamp::kMicroSecondsPerSecond;
        ++it;
    }
    if(timers_.empty())
        resetTimerfd(Timestamp());
    else
        resetTimerfd(timers_.begin()->second->expiration());

}
