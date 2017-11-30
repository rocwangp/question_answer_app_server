#pragma once

#include "CallBack.h"
#include "Buffer.h"

class EventLoop;
class Socket;

class TcpConnection : private noncopyable,
                      public std::enable_shared_from_this<TcpConnection>
{
public:
    TcpConnection(EventLoop* loop, int sockfd, const std::string name); 
    ~TcpConnection();

    EventLoop* getLoop() { return loop_; }
    
    void setConnectionCallBack(ConnectionCallBack& cb) { connectionCallBack_ = cb; }
    void setMessageCallBack(MessageCallBack& cb) { messageCallBack_ = cb ; }
    void setCloseCallBack(const CloseCallBack& cb) { closeCallBack_ = cb; }

    void connectEstablish();
    void connectDestroy();

    void shutdownWrite();
    void send(const std::string& msg);

    std::string name() { return name_; }
private:
    void handleRead();
    void handleClose();
    void handleWrite();

private:
    EventLoop* loop_;
    std::string name_;

    std::shared_ptr<Channel> channel_;
    std::unique_ptr<Socket> socket_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    CloseCallBack closeCallBack_;
    
    Buffer inputBuffer_;
    Buffer outputBuffer_;
};
