#pragma once

#include "noncopyable.h"

#include <functional>
#include <memory>


class EventLoop;
class TcpConnection;

class Channel : private noncopyable
{
    typedef std::function<void()> EventCallBack; 
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent();

    void tie(const std::shared_ptr<TcpConnection>& tcpConnection);

    void setReadCallBack(EventCallBack cb) { readCallBack_ = std::move(cb); }
    void setCloseCallBack(EventCallBack cb) { closeCallBack_ = std::move(cb); }
    void setWriteCallBack(EventCallBack cb) { writeCallBack_ = std::move(cb); }

    void set_revents(unsigned short revents) { revents_ = revents ; }
    

    int fd() { return fd_; }
    unsigned short events() { return events_; }

    void set_state(int state) { state_ = state; }
    int state() { return state_; }

    void enableReading() { events_ |= kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }

    void disableReading() { events_ &= (~kReadEvent); update(); }
    void disableWriting() { events_ &= (~kWriteEvent); update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    bool isNoneEvent() { return events_ == kNoneEvent; }
    bool isReading() { return events_ & kReadEvent; }
    bool isWriting() { return events_ & kWriteEvent; }


private:
    void update();
    void handleEventWithGuard();
private:
     
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    int fd_;
    unsigned short events_;
    unsigned short revents_;

    EventLoop* loop_;
    EventCallBack readCallBack_;
    EventCallBack closeCallBack_;
    EventCallBack writeCallBack_;

    std::weak_ptr<TcpConnection> tie_;
    bool tied_;

    int state_;
};
