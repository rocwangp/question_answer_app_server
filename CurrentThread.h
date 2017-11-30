#pragma once

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{

inline long tid()
{
    return ::syscall(SYS_gettid);
}
}
