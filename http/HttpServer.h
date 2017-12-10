#pragma once

#include "DataInfo.h" 
#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../InetAddress.h"
#include "../cppjieba/Jieba.hpp"

class HttpRequest;
class HttpResponse;
class DataBase;

class HttpServer : noncopyable
{
public:
    typedef std::unordered_map<std::string, std::string> DataInfoMap;

    HttpServer(EventLoop* loop, const InetAddress& localaddr);
    ~HttpServer();

    void start() { server_.start(); }

private:
    void onConnectionCallBack(const TcpConnectionPtr& conn);
    void onMessageCallBack(const TcpConnectionPtr& conn, Buffer& buffer);

    void handleGetMethod(const TcpConnectionPtr& conn);
    void handlePostMethod(const TcpConnectionPtr& conn);
    void handleErrorMethod(const TcpConnectionPtr& conn);
    
    std::vector<Question> queryQuestion(const std::string& keyWords);
    std::vector<Answer> queryAnswer(const std::string& questionId);
    std::vector<Comment> queryComment(const std::string& id);

    bool insertQuestion(const std::string& question, const std::string& questionDetail, int userId);
    bool insertAnswer(int questionId, const std::string& answer, int userId);
    bool insertComment(int answerId, const std::string& comment, int userId);
    bool insertUser(const std::string& account, const std::string& password, const std::string& username);

    bool checkUser(const std::string& account);
    DataInfoMap loginUser(const std::string& account, const std::string& password);

    void initStopWordMap();
    void setSignalHandler(int signo, void(*handler)(int));
    void setUpdateTask();

    void runSpider();
    void updateDataBase();

    void loggerOutputFunc(const std::string& msg);

    bool isStopWord(const string& word);
    static string currentTime();

private:
    struct ComByValue
    {
        bool operator()(const std::pair<std::string, int>& lhs, 
                        const std::pair<std::string, int>& rhs)
        {
            return lhs.second > rhs.second;
        }
    };

    static void handler(int);
private:
    
    EventLoop* loop_;
    TcpServer server_;

    std::unique_ptr<HttpRequest> httpRequest_;
    std::unique_ptr<HttpResponse> httpResponse_;
    std::unique_ptr<DataBase> dataBase_;
    std::shared_ptr<cppjieba::Jieba> jieba_;

    static pid_t pid_;
    std::atomic<off_t> loggerSize_;

    std::unordered_map<std::string, int> stopWord_;
};

