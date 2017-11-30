#include "Buffer.h"
#include "Logging.h"

#include <vector>
#include <string.h>
#include <sys/uio.h>


const int Buffer::kBufferInitSize = 4000;

Buffer::Buffer()
    : buffer_(kBufferInitSize),
      readerIndex(0),
      writerIndex(0)
{

}

Buffer::~Buffer()
{

}



int Buffer::readFd(int sockfd)
{
   char exbuf[128 * 1024];
   bzero(exbuf, sizeof(exbuf));

   int writable = writableBytes();
   std::vector<struct iovec> iovecs(2);
   iovecs[0].iov_base = &*(buffer_.begin() + writerIndex); 
   iovecs[0].iov_len = writable; 
   iovecs[1].iov_base = exbuf;
   iovecs[1].iov_len = sizeof(exbuf);

   ssize_t n = ::readv(sockfd, &*iovecs.begin(), 2);
   if(static_cast<int>(n) <= writable)
   {
       writerIndex += n;
   }
   else
   {
       writerIndex = static_cast<int>(buffer_.size());
       append(exbuf, static_cast<int>(n) - writable);
   }

   LOG_DEBUG << "read " << n << " bytes from tcp buffer";
   return n ;
}


void Buffer::append(char* buf, int len)
{
    append(static_cast<const char*>(buf), len);
}

void Buffer::append(const char* buf, int len)
{
    if(len > writableBytes())
    {
        enableWritableBytes(len);
    }
    std::copy(buf, buf + len, &*(buffer_.begin() + writerIndex));
    writerIndex += len;
}

void Buffer::enableWritableBytes(int len)
{
    buffer_.resize(buffer_.size() + len);
}


std::string Buffer::retrieve(int len)
{
    if(len > readableBytes())
        len = readableBytes();

    std::string msg(buffer_.begin() + readerIndex, buffer_.begin() + readerIndex + len);
    readerIndex += len;
    if(readerIndex == writerIndex)
    {
        readerIndex = 0;
        writerIndex = 0;
    }

    return msg;
}
