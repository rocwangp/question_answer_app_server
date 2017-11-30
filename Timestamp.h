#pragma once

#include <cstdint>

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

