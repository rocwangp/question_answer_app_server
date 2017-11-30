#pragma once

#include "LogStream.h"
#include "CallBack.h"

class Logger : private noncopyable
{


public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        ERROR,
        FATAL,
        NUMS_LOG_LEVELS
    };
    Logger(LogLevel level, std::string filename, std::string funcname, int line);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static void setOutputFunc(OutputCallBack callBack);
    static void setFlushFunc(FlushCallBack callBack);
private:
    class Impl
    {
    public:
        Impl(Logger::LogLevel level, std::string filename, std::string funcname, int line);
        ~Impl();
        void formatTime();
        void formatThreadId();
        void formatLevel();
        void finish();
        
        LogStream stream_;
        Logger::LogLevel level_;
        std::string filename_;
        std::string funcname_;
        int line_;
    };
    Impl impl_;
};


#define LOG_TRACE   Logger(Logger::TRACE, __FILE__, __func__, __LINE__).stream()
#define LOG_DEBUG   Logger(Logger::DEBUG, __FILE__, __func__, __LINE__).stream()
#define LOG_ERROR   Logger(Logger::ERROR, __FILE__, __func__, __LINE__).stream()
#define LOG_INFO    Logger(Logger::INFO, __FILE__, __func__, __LINE__).stream()
#define LOG_FATAL   Logger(Logger::FATAL, __FILE__, __func__, __LINE__).stream()
