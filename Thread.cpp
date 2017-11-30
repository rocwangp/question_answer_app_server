#include "Thread.h"

Thread::Thread()
{

}

Thread::~Thread()
{

}

void Thread::start()
{
    pthread_create(&tid_, NULL, &Thread::threadFunc, static_cast<void*>(&threadCallBack_));
    
}


void* Thread::threadFunc(void *arg)
{
    ThreadCallBack threadCallBack = *static_cast<ThreadCallBack*>(arg);
    threadCallBack();
    pthread_exit(NULL);
}
