#pragma once

#include "noncopyable.h"

#include <vector>
#include <string>

class Buffer : private noncopyable
{
public:
    Buffer();
    ~Buffer();

    int readFd(int sockfd);

    int readableBytes() { return writerIndex - readerIndex; }
    int writableBytes() { return buffer_.size() - writerIndex; }

    const char* peek() { return &*(buffer_.begin() + readerIndex); }
    void append(char* buf, int len);
    void append(const char* buf, int len);
    void enableWritableBytes(int len);

    std::string retrieve(int len);
    std::string toString() { return std::string(buffer_.begin() + readerIndex, buffer_.begin() + readableBytes()); }
private:
    static const int kBufferInitSize;

    std::vector<char> buffer_;
    int readerIndex;
    int writerIndex;
};
