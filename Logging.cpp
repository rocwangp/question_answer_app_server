#include "Logging.h"
#include "CurrentThread.h"

#include <functional>
#include <string>
#include <time.h>
#include <iostream>


void defaultOutput(const std::string& msg)
{
    fwrite(msg.c_str(), 1, msg.size(), stdout);
}

void defaultFlush()
{
    fflush(stdout);
}



static OutputCallBack g_output = std::bind(defaultOutput, std::placeholders::_1);
static FlushCallBack g_flush = std::bind(defaultFlush);

Logger::Impl::Impl(Logger::LogLevel level,
                   std::string filename,
                   std::string funcname,
                   int line)
    : level_(level),
      filename_(filename),
      funcname_(funcname),
      line_(line)
{
    formatTime();
    formatThreadId();
    formatLevel();
    stream_ << "------";
}

Logger::Impl::~Impl()
{

}

void Logger::Impl::formatTime()
{
    time_t timer;
    time(&timer);
    char timebuf[1024];
    ctime_r(&timer, timebuf);
    timebuf[strlen(timebuf) - 1] = '\0';
    stream_ << timebuf << "-";
}

void Logger::Impl::formatThreadId()
{
   stream_ << CurrentThread::tid() << "-"; 
}

void Logger::Impl::formatLevel()
{
    switch(level_)
    {
    case TRACE:
        stream_ << "TRACE" << "-";
        break;
    case DEBUG:
        stream_ << "DEBUG" << "-";
        break;
    case INFO:
        stream_ << "INFO" << "--";
        break;
    case ERROR:
        stream_ << "ERROR" << "-";
        break;
    case FATAL:
        stream_ << "FATAL" << "-";
        break;
    default:
        stream_ << "" << "------";
        break;
    }
}
void Logger::Impl::finish()
{
    stream_ << "------" << filename_ << "-" << funcname_ << "-" << line_ << "\n";
}
Logger::Logger(Logger::LogLevel level,
               std::string filename,
               std::string funcname,
               int line)
    : impl_(level, filename, funcname, line)
{

}


Logger::~Logger()
{
    impl_.finish(); 
    std::string logger = impl_.stream_.toString();
    g_output(logger);
    g_flush();
    if(impl_.level_ == FATAL)
    {
        abort();
    }
}


void Logger::setOutputFunc(OutputCallBack callBack)
{
    g_output = callBack;
}

void Logger::setFlushFunc(FlushCallBack callBack)
{
    g_flush = callBack;
}
