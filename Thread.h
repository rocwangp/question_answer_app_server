#pragma once

#include <pthread.h>

#include "noncopyable.h"
#include "CallBack.h"

class Thread : private noncopyable
{
public:
    Thread();
    ~Thread();

    void start();
    void setThreadCallBack(ThreadCallBack cb) { threadCallBack_ = std::move(cb); }

private:
    static void* threadFunc(void *arg);
private:
    pthread_t tid_;
    ThreadCallBack threadCallBack_;
};
