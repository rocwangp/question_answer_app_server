#pragma once

#include <functional>
#include <memory>
#include <string>

class TcpConnection;
class Channel;
class Buffer;
class Timestamp;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&, Buffer&)> MessageCallBack;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallBack;
typedef std::function<void()> ThreadCallBack;
typedef std::function<void()> TimerCallBack;
typedef std::function<void(const std::string&)> OutputCallBack;
typedef std::function<void()> FlushCallBack;
