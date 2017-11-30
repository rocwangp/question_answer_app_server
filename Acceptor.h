#pragma once

#include "InetAddress.h"

#include <memory>
#include <functional>

class Socket;
class EventLoop;
class Channel;

class Acceptor : private noncopyable
{
public:
    typedef std::function<void(int, const InetAddress&)> NewConnectionCallBack;
    Acceptor(EventLoop* loop, const InetAddress& addr); 
    ~Acceptor();

    void start();

    void setConnectionCallBack(NewConnectionCallBack cb) { newConnectionCallBack_ = std::move(cb); }
private:
    void handleRead();

private:
    EventLoop* loop_;
    InetAddress inetAddress_;   
    std::unique_ptr<Socket> acceptSocket_;
    std::shared_ptr<Channel> acceptChannel_;
    NewConnectionCallBack newConnectionCallBack_;

    int idleFd_;
};
