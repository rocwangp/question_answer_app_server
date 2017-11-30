#pragma once

#include "noncopyable.h"
#include <pthread.h>

class Condition : noncopyable
{
public:
    Condition(pthread_mutex_t& mutex);
    ~Condition();

public:
    void wait();
    void notify();
    void notifyAll();

private:
    pthread_mutex_t& mutex_;
    pthread_cond_t cond_;
};
