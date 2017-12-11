#pragma once

#include "../noncopyable.h"

#include <string>
#include <memory>
#include <unordered_map>

class HttpRequest : noncopyable
{
public:
    typedef std::unordered_map<std::string, std::string> ArgumentMap;

    enum HttpMethod
    {
        GET,
        POST,
        PUT,
        DELETE,
        NONE,
        NUMS_METHOD
    };

    enum HttpType
    {
        QUERY_QUESTION = 0,
        QUERY_ANSWER = 1,
        QUERY_COMMENT = 2,
        QUERY_USER = 3,
        INSERT_QUESTION = 4,
        INSERT_ANSWER = 5,
        INSERT_COMMENT = 6,
        INSERT_USER = 7,
        CHECK_USER = 8,
        LOGIN_USER = 9,
        SEARCH_USER = 10,
        QUERY_FANS = 11,
        QUERY_NOADOPTEDQUESTION = 12,
        UNKNOW_TYPE, 
        NUMS_TYPE
    };

    HttpRequest();
    ~HttpRequest();

    void parseHeader(const std::string& header);
    ArgumentMap getArguments() const;

    std::string method() const;
    std::string source() const;
    std::string host() const;
    std::string contentType() const;
    std::string userAgent() const;
    double version() const;
    uint64_t contentLength() const;
    
    bool isValid() const;
    bool isKeepAlive() const;

    bool isGetMethod() const;
    bool isPostMethod() const;

    bool isQueryQuestion() const;
    bool isQueryAnswer() const;
    bool isQueryComment() const;
    bool isQueryUser() const;
    bool isQueryFans() const;

    bool isInsertQuestion() const;
    bool isInsertAnswer() const;
    bool isInsertComment() const;
    bool isInsertUser() const;
    
    /* 注册账号时核查账户名是否被使用 */
    bool isCheckUser() const;
    /* 登录账号时验证账号和密码，返回账户信息 */
    bool isLoginUser() const;

    bool isSearchUser() const;
    bool isQueryNoAdoptedQuestion() const;

private:
    static std::string kDefaultContentType;
    
    class Impl;
    std::unique_ptr<Impl> impl_;
};

