#pragma once

#include "./noncopyable.h"
#include "Timestamp.h"
#include "CallBack.h"

class Timer : noncopyable
{
public:
    Timer(Timestamp when, TimerCallBack callBack, double interval)
        : expiration_(when),
          callBack_(callBack),
          interval_(interval),
          repeat_(interval_ > 0.0)
    {

    }

    void run() { callBack_(); }
    bool isRepeat() const { return repeat_; }
    const Timestamp expiration() { return expiration_; }

    void resetTimer(const Timestamp& now) 
    { expiration_.resetTime(static_cast<int64_t>(now.microSecondsSinceEpoch() + 
                                                 interval_ * Timestamp::kMicroSecondsPerSecond)); }
private:
    Timestamp expiration_;
    TimerCallBack callBack_;
    const double interval_;
    const bool repeat_;
};
