#include <iostream>
#include <string>
#include <string.h>

#include "../HttpRequest.h"
#include "../StringUtil.h"
#include "../../Logging.h"
using namespace std;



HttpRequest httpRequest;
string headers = "%s %s HTTP/%s\r\n\
Host: %s\r\n\
User-Agent: %s\r\n\
Content-Type: %s\r\n\
Content-Length: %d\r\n\
Connection: %s\r\n\
\r\n\
{\"typeId\":\"%s\", \"%s\":\"%s\"}";

void test(string method, string source, 
          string version, string host, 
          string userAgent, string contentType, 
          uint64_t contentLength, string connection, 
          string typeId, string key, string value)
{
    char buf[4096];
    bzero(buf, sizeof(buf));
    sprintf(buf, headers.c_str(), 
            method.c_str(),    source.c_str(), 
            version.c_str(),   host.c_str(), 
            userAgent.c_str(), contentType.c_str(), 
            contentLength,     connection.c_str(), 
            typeId.c_str(),    key.c_str(), value.c_str());
    
    string requestHeader("");
    requestHeader.assign(buf, strlen(buf));
    cout << requestHeader << endl;
    httpRequest.parseHeader(requestHeader);

    if(httpRequest.method() != StringUtil::toLower(method))
        cout << "parse method wrong!!!!!!!!!!!" << endl;
    if(httpRequest.version() != StringUtil::toDouble(version))
        cout << "parse version wrong!!!!!!!!!!" << endl;
    if(!httpRequest.isKeepAlive())
        cout << "parse Keep-Alive wrong!!!!!!!" << endl;
    if(httpRequest.source() != source)
        cout << "parse source wrong!!!!!!!!!!!" << endl;
    if(httpRequest.contentLength() != contentLength)
        cout << "parse ContentLength wrong!!!!" << endl;
    if(httpRequest.userAgent() != userAgent)
        cout << "parse UserAgent wrong!!!!!!!!" << endl;
    if(httpRequest.contentType() != contentType)
        cout << "parse ContentType wrong!!!!!!" << endl;
    HttpRequest::ArgumentMap argumentMap = httpRequest.getArguments();
    if(typeId == "0" && !httpRequest.isQueryQuestion())
        cout << "parse Argument TypeId wrong!!" << endl;
    if(typeId == "1" && !httpRequest.isQueryAnswer())
        cout << "parse Argument TypeId wrong!!" << endl;
    if(typeId == "2" && !httpRequest.isQueryComment())
        cout << "parse Argument TypeId wrong!!" << endl;
    if(typeId == "3" && !httpRequest.isQueryUser())
        cout << "parse Argument TypeId wrong!!" << endl;
    if(argumentMap.find(key) == argumentMap.end() || argumentMap[key] != value)
        cout << "parse Argument key wrong!!!!!!!!!" << endl;
    LOG_INFO << "done";
}
int main()
{
   
    
    string method = "POST";
    string source = "/";
    string version = "1.1";
    string host = "39.106.114.141";
    string userAgent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022)";
    string contentType = "application/x-www-form-urlencoded";
    uint64_t contentLength = 40;
    string connection = "Keep-Alive";
    string typeId = "0";
    string key = "question";
    string value = "中国足球的未来怎么样";

    test(method, source, version, host, userAgent, contentType, contentLength, connection, typeId, key, value); 
    method = "GET";
    test(method, source, version, host, userAgent, contentType, contentLength, connection, typeId, key, value); 
    method = "POST";
    typeId = "1";
    key = "answerIds";
    value = "1,2,3,4";
    test(method, source, version, host, userAgent, contentType, contentLength, connection, typeId, key, value); 
    return 0;
}

