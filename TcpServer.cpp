#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "SocketsOps.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Logging.h"

#include <sstream>

TcpServer::TcpServer(EventLoop* loop, const InetAddress& inetAddress)
    : loop_(loop),
      acceptor_(new Acceptor(loop_, inetAddress)),
      eventLoopThreadPool_(new EventLoopThreadPool(loop))
{
    acceptor_->setConnectionCallBack(std::bind(&TcpServer::newConnection, this, std::placeholders::_1));
}


TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
    eventLoopThreadPool_->start();
    acceptor_->start();
}


void TcpServer::newConnection(int sockfd)
{
    LOG_TRACE << "recv a new connection";

    EventLoop* ioloop = eventLoopThreadPool_->nextLoop();

    InetAddress peerAddress, localAddress;
    sockets::getPeerAddress(sockfd, peerAddress);
    sockets::getLocalAddress(sockfd, localAddress);

    std::stringstream name;
    name << sockfd << "-" 
         << localAddress.toIpString() << ":" << localAddress.toPortString() << "-"
         << peerAddress.toIpString() << ":" << peerAddress.toPortString();

    LOG_DEBUG << "new connection's name is " << name.str();
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioloop, sockfd, name.str());

    connections_[name.str()] = conn;

    conn->setConnectionCallBack(connectionCallBack_);
    conn->setMessageCallBack(messageCallBack_);
    conn->setCloseCallBack(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioloop->runInLoop(std::bind(&TcpConnection::connectEstablish, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}


void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    LOG_DEBUG << "remove connection " << conn->name();
    connections_.erase(conn->name());
    EventLoop* ioloop = conn->getLoop();
    ioloop->runInLoop(std::bind(&TcpConnection::connectDestroy, conn));
}
