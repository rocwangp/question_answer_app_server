#pragma once

#include "noncopyable.h"

#include <functional>

class ErrorGuard : noncopyable
{
public:
    explicit ErrorGuard(std::function<void()> quitCallBack)
        : quitCallBack_(quitCallBack),
          dismiss_(false)
    {  }

    ~ErrorGuard()
    {
        if(!dismiss_)
            quitCallBack_();
    }

    void dismiss()
    {
        dismiss_ = true;
    }

private:
    std::function<void()> quitCallBack_;
    bool dismiss_;
};
