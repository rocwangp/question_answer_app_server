#include "Timestamp.h"

#include <sys/time.h>

const int Timestamp::kMicroSecondsPerSecond = 1000000;

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = static_cast<int64_t>(tv.tv_sec);
    return Timestamp(seconds * Timestamp::kMicroSecondsPerSecond + tv.tv_usec / 1000);
}

