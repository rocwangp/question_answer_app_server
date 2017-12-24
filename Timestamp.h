#pragma once

#include "StringUtil.h"

#include <cstdint>
#include <string>

class Timestamp
{
public:

    Timestamp()
        : microSecondsSinceEpoch_(0)
    {

    }
    explicit Timestamp(int64_t microSecondsSinceEpoch)
        : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {

    }

    Timestamp(const Timestamp& timestamp)
        : microSecondsSinceEpoch_(timestamp.microSecondsSinceEpoch_)
    {

    }

    static std::string currentTime()
    {
        time_t curTime = time(nullptr); 
        struct tm curtm = *localtime(&curTime);
        std::string date = StringUtil::toString(curtm.tm_year + 1900) + "-"
                    + StringUtil::toString(curtm.tm_mon) + "-"
                    + StringUtil::toString(curtm.tm_mday) + ":"
                    + StringUtil::toString(curtm.tm_hour) + "::"
                    + StringUtil::toString(curtm.tm_min);
        return date;
    }

    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    
    void resetTime(int64_t microSecondsSinceEpochArg) { microSecondsSinceEpoch_ = microSecondsSinceEpochArg; }
    static Timestamp now();
    static const int kMicroSecondsPerSecond;
private:
    int64_t microSecondsSinceEpoch_; 
};


inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}


inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator>=(const Timestamp& lhs, const Timestamp& rhs)
{
    return !(lhs < rhs);
}

