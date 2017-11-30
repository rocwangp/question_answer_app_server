#include "MutexLock.h"

MutexLock::MutexLock()
{
    pthread_mutex_init(&mutex_, NULL);
}

MutexLock::~MutexLock()
{
    pthread_mutex_destroy(&mutex_);
}
