#include "HttpResponse.h"

#include "../Logging.h"
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/prettywriter.h"

#include <sstream>
#include <ctime>
#include <cstring>

using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;

class HttpResponse::Impl
{
public:
    Impl();
    ~Impl();

    void formatDate();
    
    double version_;
    int    code_;
    string state_;
    string date_;
    string contentType_;
};

HttpResponse::Impl::Impl()
    : version_(1.1),
      code_(200),
      state_("OK"),
      contentType_("text/html; charset=UTF-8") 
{  }

HttpResponse::Impl::~Impl()
{

}

void HttpResponse::Impl::formatDate()
{
    time_t timer;
    ::time(&timer);
    char timerbuf[1024];
    ctime_r(&timer, timerbuf);
    timerbuf[strlen(timerbuf) - 1] = '\0';
    date_.assign(timerbuf, strlen(timerbuf));
}

HttpResponse::HttpResponse()
    : impl_(new Impl())
{
    
}

HttpResponse::~HttpResponse()
{

}


void HttpResponse::setVersion(double version)
{
    impl_->version_ = version;
}

void HttpResponse::setCode(int code)
{
    impl_->code_ = code;
    switch(code)
    {
    case 200:
        impl_->state_ = "OK";
        break;
    case 404:
        impl_->state_ = "Not Found";
        break;
    case 400:
        impl_->state_ = "Bad Request";
        break;
    case 401:
        impl_->state_ = "Unauthorized";
        break;
    case 403:
        impl_->state_ = "Forbidden";
        break;
    case 500:
        impl_->state_ = "Internal Server Error";
        break;
    case 503:
        impl_->state_ = "Server Unavailable";
        break;
    default:
        break;
    }
}


void HttpResponse::setContentType(const string& contentType)
{
    impl_->contentType_ = contentType;
}


string HttpResponse::generateResponse(const BodyInfoMapList& bodyMapList)
{
    impl_->formatDate();

    stringstream oss;
    oss << "HTTP/1.0" << " " << impl_->code_ << " " << impl_->state_ << "\r\n";
    oss << "Date: " << impl_->date_ << "\r\n";
    oss << "Content-Type: " << impl_->contentType_ << "\r\n";
    
    using namespace rapidjson;
    Document doc;
    doc.SetObject();
    Document::AllocatorType &allocator = doc.GetAllocator();
    
    Value infoArray(kArrayType);
    for(const BodyInfoMap &bodyMap : bodyMapList)
    {
        Value infoMap(kObjectType);
        for(auto it = bodyMap.begin(); it != bodyMap.end(); ++it)
        {
            infoMap.AddMember(StringRef(it->first.c_str()), StringRef(it->second.c_str()), allocator);
        }
        infoArray.PushBack(infoMap, allocator);
    }
    doc.AddMember("information", infoArray, allocator);
    StringBuffer buffer;
    PrettyWriter<StringBuffer> prettyWriter(buffer);
    doc.Accept(prettyWriter);

    string body = buffer.GetString();
    oss << "Content-Length: " << body.length() << "\r\n";
    oss << "\r\n";
    oss << body;
    
    return oss.str();
}

