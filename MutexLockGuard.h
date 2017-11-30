#pragma once

#include "noncopyable.h"

#include <pthread.h>

class MutexLockGuard : private noncopyable
{
public:
    MutexLockGuard(pthread_mutex_t& mutex)
        : mutex_(mutex)
    {
        pthread_mutex_lock(&mutex_);
    }

    ~MutexLockGuard()
    {
        pthread_mutex_unlock(&mutex_);
    }

private:
    pthread_mutex_t& mutex_;
};
