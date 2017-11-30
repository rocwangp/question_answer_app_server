#pragma once

#include "../noncopyable.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class HttpResponse : noncopyable
{
public:
    typedef std::unordered_map<std::string, std::string> BodyInfoMap;
    typedef std::vector<BodyInfoMap> BodyInfoMapList;
    HttpResponse();
    ~HttpResponse();

    void setVersion(double version);
    void setCode(int code);
    void setContentType(const std::string& contentType);
    std::string generateResponse(const BodyInfoMapList& bodyMapList);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

