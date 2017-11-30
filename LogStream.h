#pragma once

#include "Buffer.h"

#include <sstream>
#include <cstring>

class LogStream : private noncopyable
{
public:
    typedef LogStream self;


    std::string toString()
    {
        return buffer_.toString();
    }
    self& operator<<(const char* msg)
    {
        buffer_.append(msg, strlen(msg));
        return *this;
    }

    self& operator<<(char* msg)
    {
        return this->operator<<(static_cast<const char*>(msg));
    }

    self& operator<<(const std::string& msg)
    {
        return this->operator<<(msg.c_str());
    }
    
    self& operator<<(std::string& msg)
    {
        return this->operator<<(static_cast<const std::string&>(msg));
    }
    self& operator<<(long long int n)
    {
        std::stringstream oss;
        oss << n;
        return this->operator<<(oss.str());
    }
    self& operator<<(uint64_t n)
    {
        return this->operator<<(static_cast<long long int>(n));
    }

    self& operator<<(int n)
    {
        return this->operator<<(static_cast<long long int>(n));
    }
    self& operator<<(long n)
    {
        return this->operator<<(static_cast<long long int>(n));
    }
   
    self& operator<<(unsigned int n)
    {
        return this->operator<<(static_cast<long long int>(n));
    }

    self& operator<<(double n)
    {
        std::stringstream oss;
        oss << n;
        return this->operator<<(oss.str());
    }

private:
    Buffer buffer_;
};
