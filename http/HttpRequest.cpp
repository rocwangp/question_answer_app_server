#include "HttpRequest.h"

#include "../StringUtil.h"
#include "../ErrorGuard.h"
#include "../Logging.h"
#include "../rapidjson/document.h"
#include "../rapidjson/stringbuffer.h"
#include "../rapidjson/prettywriter.h"

#include <algorithm>
#include <vector>
#include <regex>
#include <functional>
#include <cstdint>
#include <cassert>

using std::string;
using std::stringstream;

string HttpRequest::kDefaultContentType = "";

class HttpRequest::Impl
{
public:

    Impl();
    ~Impl();

    void initFormatFuncList();
    void clear();
    void parseHeader(const string& header);
    void parseRequestHeader(const string& header, const string& pattern);

    void formatMethod(const string& header);
    void formatSource(const string& header);
    void formatVersion(const string& header);
    void formatHost(const string& header);
    void formatUserAgent(const string& header);
    void formatAccept(const string& header);
    void formatReferer(const string& header);
    void formatAcceptEncoding(const string& header);
    void formatAcceptLanguage(const string& header);
    void formatContentType(const string& header);
    void formatContentLength(const string& header);
    void formatConnection(const string& header);
    void formatArgument(const string& header);
    
    typedef std::function<void(const string&)> FormatFunc;
    typedef std::vector<FormatFunc> FormatFuncList;
    typedef std::vector<string> AcceptEncodingList;
    typedef std::vector<string> AcceptLanguageList;
    
    HttpRequest::HttpMethod method_;
    HttpRequest::HttpType type_;
    string       source_;
    ArgumentMap  arguments_; 
    double       version_;
    string       host_;
    string       userAgent_;
    string       accept_;
    string       referer_;
    AcceptLanguageList acceptLanguage_;
    AcceptEncodingList acceptEncoding_;
    string       connection_;
    string       cookie_;
    string       contentType_;
    uint64_t     contentLength_;
    bool         requestValid_;
    FormatFuncList formatFuncs_;
    static const std::vector<string> kHeaderPatterns;

};

const std::vector<string> HttpRequest::Impl::kHeaderPatterns = {
    "Host", 
    "User-Agent", 
    "Accept", 
    "Referer", 
    "Accept-Encoding", 
    "Accept-Language",
    "Content-Type", 
    "Content-Length", 
    "Connection",
    "Argument"
};

HttpRequest::Impl::Impl()
    : method_(HttpRequest::NONE),
      source_(""),
      version_(0),
      host_(""),
      userAgent_(""),
      accept_(""),
      referer_(""),
      connection_(""),
      cookie_(""),
      contentType_(""),
      contentLength_(0),
      requestValid_(true)
{
    initFormatFuncList();
}


HttpRequest::Impl::~Impl()
{

}

void HttpRequest::Impl::initFormatFuncList()
{
    using std::placeholders::_1;
    formatFuncs_ = {
        std::bind(&Impl::formatMethod, this, _1),
        std::bind(&Impl::formatSource, this, _1),
        std::bind(&Impl::formatVersion, this, _1),
        std::bind(&Impl::formatHost, this, _1),
        std::bind(&Impl::formatUserAgent, this, _1),
        std::bind(&Impl::formatAccept, this, _1),
        std::bind(&Impl::formatReferer, this, _1),
        std::bind(&Impl::formatAcceptEncoding, this, _1),
        std::bind(&Impl::formatAcceptLanguage, this, _1),
        std::bind(&Impl::formatContentType, this, _1),
        std::bind(&Impl::formatContentLength, this, _1),
        std::bind(&Impl::formatConnection, this, _1),
        std::bind(&Impl::formatArgument, this, _1)
    };
}
void HttpRequest::Impl::clear()
{
    method_ = HttpRequest::HttpMethod::NONE;
    type_ = HttpRequest::HttpType::UNKNOW_TYPE;
    source_.clear();
    arguments_.clear();
    version_ = 0;
    host_.clear();
    userAgent_.clear();
    accept_.clear();
    referer_.clear();
    acceptLanguage_.clear();
    acceptEncoding_.clear();
    connection_.clear();
    cookie_.clear();
    contentType_.clear();
    contentLength_ = 0;
    requestValid_ = true;
}

void HttpRequest::Impl::formatMethod(const string& header)
{
    string pattern = "([A-Z]+) +[^ ]+ HTTPS?/[0-9](\\.[0-9])?\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        string method = StringUtil::toLower(results[1]);
        LOG_INFO << results.str() << " " << results[1];
        LOG_INFO << method;
        if(method == "get")
            method_ = HttpRequest::HttpMethod::GET;
        else if(method == "post")
            method_ = HttpRequest::HttpMethod::POST;
        else if(method == "put")
            method_ = HttpRequest::HttpMethod::PUT;
        else if(method == "delete")
            method_ = HttpRequest::HttpMethod::DELETE;

        LOG_INFO << "http request method is " << method;
    }
    else
    {
        LOG_INFO << "http request method is incorrent";
        requestValid_ = false;
    }
}

void HttpRequest::Impl::formatSource(const string& header)
{
    string pattern = "[GET|POST|PUT|DELETE] +([^ ]+) HTTPS?/[0-9](\\.[0-9])?\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        source_ = results[1];
        LOG_INFO << "http request source is " << source_;
    }
}

void HttpRequest::Impl::formatVersion(const string& header)
{
    string pattern = "[GET|POST|PUT|DELETE] +[^ ]+ HTTPS?/([0-9](\\.[0-9])?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        version_ = StringUtil::toDouble(results[1]);
        LOG_INFO << "http request version is " << version_;
    }
    else
    {
        requestValid_ = false;
    }
}

void HttpRequest::Impl::formatHost(const string& header)
{
    string pattern = "HOST *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        host_ = results[1];
        LOG_INFO << "http request host is " << host_;
    }

}
void HttpRequest::Impl::formatUserAgent(const string& header)
{
    string pattern = "User-Agent *: *([\\w|\\s|\\.|\\(|\\)|,|;|/]+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        userAgent_ = results[1];
        LOG_INFO << "http request User-Agent is " << userAgent_;
    }
}

void HttpRequest::Impl::formatAccept(const string& header)
{
    string pattern = "Accept *: *([\\w|/|,|\\*|;|=|\\.]+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        accept_ = results[1];
        LOG_INFO << "http request Accept is " << accept_;
    }
}

void HttpRequest::Impl::formatReferer(const string& header)
{
    string pattern = "Referer *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        referer_ = results[1];
        LOG_INFO << "http request Referer is " << referer_;
    }
}

void HttpRequest::Impl::formatAcceptEncoding(const string& header)
{
    string pattern = "Accept-Encoding *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        acceptEncoding_ = StringUtil::split(results[1], ',');
        string acceptEncoding("");
        std::for_each(acceptEncoding_.begin(), 
                      acceptEncoding_.end(),
                      [&acceptEncoding](const string& s) { acceptEncoding += s + " "; });
        LOG_INFO << "http request AcceptEncoding is " << acceptEncoding;
    }
}

void HttpRequest::Impl::formatAcceptLanguage(const string& header)
{
    string pattern = "Accept-Language *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        acceptLanguage_ = StringUtil::split(results[1], ',');
        string acceptLanguage("");
        std::for_each(acceptLanguage_.begin(),
                      acceptLanguage_.end(),
                      [&acceptLanguage](const string& s) { acceptLanguage += s + " "; });
        LOG_INFO << "http request AcceptLanguage is " << acceptLanguage;
    }
}

void HttpRequest::Impl::formatContentType(const string& header)
{
    string pattern = "Content-Type *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        contentType_ = results[1];
        LOG_INFO << "http request Content-Type is " << contentType_;
    }
}

void HttpRequest::Impl::formatContentLength(const string& header)
{
    string pattern = "Content-Length *: *([1-9][0-9]+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        contentLength_ = StringUtil::toUInt64(results[1]);
        LOG_INFO << "http request Content-Length is " << contentLength_;
    }
}

void HttpRequest::Impl::formatConnection(const string& header)
{
    string pattern = "Connection *: *(.+?)\r\n";
    std::regex re(pattern, std::regex::icase);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        connection_ = results[1];
        LOG_INFO << "http request Connection is " << connection_;
    }
}

void HttpRequest::Impl::formatArgument(const string& header)
{
    string pattern = "\r\n\r\n(.*)";
    std::regex re(pattern);
    std::smatch results;
    if(regex_search(header, results, re))
    {
        LOG_INFO << "http request Argument is " << results[1];
        using namespace rapidjson;
        Document document;
        document.Parse<0>(results[1].str().c_str());
        if(document.HasParseError() || !document.HasMember("typeId"))
        {
            requestValid_ = false;
            return;
        }
        for(rapidjson::Value::ConstMemberIterator it = document.MemberBegin();
            it != document.MemberEnd(); ++it)
        {
            if(!it->value.IsString())
            {
                requestValid_ = false;
                return;
            }
        }
        if(document.HasMember("typeId"))
        {
            int typeId = StringUtil::toInt(document["typeId"].GetString());
            LOG_INFO << typeId;
            if(typeId < HttpRequest::HttpType::NUMS_TYPE)
                type_ = HttpRequest::HttpType(typeId);
        }
        
        if(type_ == HttpRequest::HttpType::QUERY_QUESTION && 
           document.HasMember("question"))
        {
            LOG_INFO << "query question";
            arguments_["question"] = document["question"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::QUERY_ANSWER && 
                document.HasMember("answerIds"))
        {
            arguments_["answerIds"] = document["answerIds"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::QUERY_COMMENT && 
                document.HasMember("commentIds"))
        {
            arguments_["commentIds"] = document["commentIds"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::QUERY_USER && 
                document.HasMember("userId"))
        {
            arguments_["userId"] = document["userId"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::QUERY_FANS &&
                document.HasMember("fansIds"))
        {
            arguments_["fansIds"] = document["fansIds"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::QUERY_NOADOPTEDQUESTION)
        {
            
        }
        else if(type_ == HttpRequest::HttpType::INSERT_QUESTION && 
                document.HasMember("question") &&
                document.HasMember("userId"))
        {
            arguments_["question"] = document["question"].GetString();
            if(document.HasMember("questionDetail"))
                arguments_["questionDetail"] = document["questionDetail"].GetString();
            else
                arguments_["questionDetail"] = "";
            arguments_["userId"] = document["userId"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::INSERT_ANSWER && 
                document.HasMember("questionId") && 
                document.HasMember("answer") &&
                document.HasMember("userId"))
        {
            arguments_["questionId"] = document["questionId"].GetString();
            arguments_["answer"] = document["answer"].GetString();
            arguments_["userId"] = document["userId"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::INSERT_COMMENT &&
                document.HasMember("answerId") && 
                document.HasMember("comment") &&
                document.HasMember("userId"))
        {
            arguments_["answerId"] = document["answerId"].GetString();
            arguments_["comment"] = document["comment"].GetString();
            arguments_["userId"] = document["userId"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::INSERT_USER &&
                document.HasMember("username") &&
                document.HasMember("password"))
        {
            arguments_["username"] = document["username"].GetString();
            arguments_["password"] = document["password"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::CHECK_USER &&
                document.HasMember("username"))
        {
            arguments_["username"] = document["username"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::LOGIN_USER &&
                document.HasMember("username") &&
                document.HasMember("password"))
        {
            arguments_["username"] = document["username"].GetString();
            arguments_["password"] = document["password"].GetString();
        }
        else if(type_ == HttpRequest::HttpType::SEARCH_USER && 
                document.HasMember("nickname"))
        {
            arguments_["nickname"] = document["nickname"].GetString();
        }
        else
        {

        }
    }
    else
    {
        requestValid_ = false;
    }
}


void HttpRequest::Impl::parseHeader(const string& header)
{
    clear();
    for(auto formatFunc : formatFuncs_)
    {
        formatFunc(header);
        if(!requestValid_)
            break;
    }
}


HttpRequest::HttpRequest()
    : impl_(new Impl())
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::parseHeader(const string& header)
{
    impl_->parseHeader(header);
}

HttpRequest::ArgumentMap HttpRequest::getArguments() const
{
    return impl_->arguments_;
}

string HttpRequest::method() const
{
    if(impl_->method_ == HttpMethod::GET)
        return "get";
    else if(impl_->method_ == HttpMethod::POST)
        return "post";
    else if(impl_->method_ == HttpMethod::PUT)
        return "put";
    else if(impl_->method_ == HttpMethod::DELETE)
        return "delete";
    else if(impl_->method_ == HttpMethod::NONE)
        return "none";
    else
        return "";
}

string HttpRequest::host() const
{
    return impl_->host_;
}

string HttpRequest::source() const
{
    return impl_->source_;
}

string HttpRequest::contentType() const
{
    if(impl_->contentType_.empty())
        return kDefaultContentType;
    else
        return impl_->contentType_;
}

string HttpRequest::userAgent() const
{
    return impl_->userAgent_;
}
double HttpRequest::version() const
{
    return impl_->version_;
}

uint64_t HttpRequest::contentLength() const
{
    return impl_->contentLength_;
}
bool HttpRequest::isValid() const
{
    return impl_->requestValid_;
}

bool HttpRequest::isKeepAlive() const
{
    return StringUtil::toLower(impl_->connection_) == "keep-alive";
}

bool HttpRequest::isGetMethod() const
{
    return impl_->method_ == HttpMethod::GET && impl_->requestValid_;
}

bool HttpRequest::isPostMethod() const
{
    return impl_->method_ == HttpMethod::POST && impl_->requestValid_;
}

bool HttpRequest::isQueryQuestion() const
{
    return impl_->type_ == HttpType::QUERY_QUESTION;
}

bool HttpRequest::isQueryAnswer() const
{
    return impl_->type_ == HttpType::QUERY_ANSWER;
}

bool HttpRequest::isQueryComment() const
{
    return impl_->type_ == HttpType::QUERY_COMMENT;
}

bool HttpRequest::isQueryUser() const
{
    return impl_->type_ == HttpType::QUERY_USER;
}

bool HttpRequest::isQueryFans() const
{
    return impl_->type_ == HttpType::QUERY_FANS;
}

bool HttpRequest::isInsertQuestion() const
{
    return impl_->type_ == HttpType::INSERT_QUESTION;
}

bool HttpRequest::isInsertAnswer() const 
{
    return impl_->type_ == HttpType::INSERT_ANSWER;
}

bool HttpRequest::isInsertComment() const
{
    return impl_->type_ == HttpType::INSERT_COMMENT;
}

bool HttpRequest::isInsertUser() const
{
    return impl_->type_ == HttpType::INSERT_USER;
}

bool HttpRequest::isCheckUser() const
{
    return impl_->type_ == HttpType::CHECK_USER; 
}

bool HttpRequest::isLoginUser() const
{
    return impl_->type_ == HttpType::LOGIN_USER;
}

bool HttpRequest::isSearchUser() const
{
    return impl_->type_ == HttpType::SEARCH_USER;
}

bool HttpRequest::isQueryNoAdoptedQuestion() const
{
    return impl_->type_ == HttpType::QUERY_NOADOPTEDQUESTION;
}
