#pragma once

#include "noncopyable.h"

#include <pthread.h>

class MutexLock : private noncopyable
{
public:
    MutexLock();
    ~MutexLock();

    pthread_mutex_t& mutex() { return mutex_; }

private:
    pthread_mutex_t mutex_;
};
