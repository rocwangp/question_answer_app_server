#pragma once

#include "TcpConnection.h"

#include <map>
#include <string>

class EventLoop;
class InetAddress;
class Acceptor;
class EventLoopThreadPool;

class TcpServer : private noncopyable
{
public:

    TcpServer(EventLoop* loop, const InetAddress& addr);
    ~TcpServer();

    void start();
    
    void stop() {}
    void setConnectionCallBack(ConnectionCallBack cb) { connectionCallBack_ = cb; }
    void setMessageCallBack(MessageCallBack cb) { messageCallBack_ = cb; }

private:
    void newConnection(int sockfd);
    
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

private:
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

    EventLoop *loop_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;
    ConnectionMap connections_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
};
