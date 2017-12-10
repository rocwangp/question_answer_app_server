#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "DataBase.h"
#include "StringUtil.h"
#include "../Logging.h"

#include <fstream>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

using std::placeholders::_1;
using std::placeholders::_2;

using std::string;
using std::vector;
using std::unordered_map;
using std::pair;

static const char* const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
static const char* const IDF_PATH = "../cppjieba/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";

static const string USER_NAME = "root";
static const string PASS_WORD = "3764819";
static const string DATA_BASE_NAME = "app";
static const string WORD_TABLE_NAME = "word";
static const string QUESTION_TABLE_NAME = "question";
static const string ANSWER_TABLE_NAME = "answer";
static const string COMMENT_TABLE_NAME = "comment";
static const string USER_TABLE_NAME = "user";
static const int PORT_NUM = 3306;

pid_t HttpServer::pid_ = -1;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& localaddr)
    : loop_(loop),
      server_(loop, localaddr),
      httpRequest_(new HttpRequest()),
      httpResponse_(new HttpResponse()),
      dataBase_(new DataBase(USER_NAME, PASS_WORD, DATA_BASE_NAME, PORT_NUM)),
      jieba_(std::make_shared<cppjieba::Jieba>(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH)),
      loggerSize_(0)
{
    server_.setConnectionCallBack(
                    std::bind(&HttpServer::onConnectionCallBack, this, _1));
    server_.setMessageCallBack(
                    std::bind(&HttpServer::onMessageCallBack, this, _1, _2));
    
    Logger::setOutputFunc(std::bind(&HttpServer::loggerOutputFunc, this, std::placeholders::_1));

    setSignalHandler(SIGCHLD, &HttpServer::handler);

    setUpdateTask(); 

    initStopWordMap();
}

HttpServer::~HttpServer()
{
   
}

void HttpServer::initStopWordMap()
{
    std::ifstream fin(STOP_WORD_PATH, std::ios_base::in);
    if(!fin.is_open())
    {
        LOG_ERROR << "load stop word error, fail to open the file";
        return;
    }
    
    /* 加载停用词表 */
    string word("");
    while(!fin.eof())
    {
        std::getline(fin, word);
        stopWord_[word] = 1;
    }

    fin.close();
    LOG_INFO << "load stop word success";
    return;
}

bool HttpServer::isStopWord(const string& word)
{
    return stopWord_.find(word) != stopWord_.end();
}

/* FIXME: 改成滚动日志 */
void HttpServer::loggerOutputFunc(const string& msg)
{
    FILE* file = nullptr;
    off_t  oneHundredMB = 100 * 1024 * 1024;
    if(loggerSize_ >= oneHundredMB)
    {
        loggerSize_ = 0;
        file = fopen("log.txt", "w");
    }
    else
    {
        file = fopen("log.txt", "a");
    }
    fwrite(msg.c_str(), 1, msg.size(), file);
    fflush(file);
    fclose(file);
    loggerSize_ += msg.size();
}

/* 信号处理函数，用于响应爬虫进程的SIGCHLD信号，防止僵尸进程 */
void HttpServer::handler(int)
{
    int stat;
    struct sigaction act;
    act.sa_handler = &HttpServer::handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);
    while(waitpid(-1, &stat, WNOHANG) > 0)
        ;
    pid_ = -1;
    LOG_DEBUG << "spider close";
}

void HttpServer::setSignalHandler(int signo, void(*handler)(int))
{
    struct sigaction act;
    if(handler == nullptr)
        act.sa_handler = SIG_IGN;
    else
        act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(signo, &act, nullptr);
    LOG_INFO << "set SIGCHLD signal handler success";
}

/* 每天2点开始爬虫，3点更新数据库 */
void HttpServer::setUpdateTask()
{
    time_t curtime = time(NULL);
    struct tm tartm = *localtime(&curtime);
    if(tartm.tm_hour >= 2)
        tartm.tm_mday += 1;

    tartm.tm_hour = 2;
    tartm.tm_min = 0;
    tartm.tm_sec = 0;

    /* microSeconds是绝对时间 */ 
    int64_t microSeconds = static_cast<int64_t>(mktime(&tartm) * Timestamp::kMicroSecondsPerSecond);
    Timestamp spiderWhen(microSeconds);
    /* runAt第三个参数是周期间隔，每隔一天调用一次 */
    loop_->runAt(spiderWhen, 
                 std::bind(&HttpServer::runSpider, this),
                 24 * 60 * 60);

    tartm.tm_hour += 1;
    microSeconds = static_cast<int64_t>(mktime(&tartm) * Timestamp::kMicroSecondsPerSecond);
    Timestamp dataBaseWhen(microSeconds);
    loop_->runAt(dataBaseWhen, 
                 std::bind(&HttpServer::updateDataBase, this),
                 24 * 60 * 60);
    LOG_INFO << "set UpdateTask success";
}

/* 开子进程调用爬虫，导致需要在多线程中处理信号，XXX: 有什么不用进程的办法？ */
void HttpServer::runSpider()
{
    LOG_INFO << "run spider";
    if((pid_ = fork()) == 0)
    {
        execl("./spider.py", "spider.py", static_cast<char*>(0));
    }
}

/* 关闭爬虫进程，更新数据库 */
void HttpServer::updateDataBase()
{
    if(pid_ != -1)
        kill(pid_, SIGQUIT);
    LOG_INFO << "update database";
    dataBase_->updateDataBase("zhihu_data.txt", jieba_, stopWord_);
}

void HttpServer::onConnectionCallBack(const TcpConnectionPtr&)
{
    LOG_DEBUG << "connect up";
}

void HttpServer::onMessageCallBack(const TcpConnectionPtr& conn, Buffer& buffer)
{
    /* 从缓冲区中取出所有数据，通常是一次请求的数据 */
    string header = buffer.retrieve(buffer.readableBytes()); 
    LOG_INFO << header;
    httpRequest_->parseHeader(header);
    
    if(httpRequest_->isGetMethod())
    {
        handleGetMethod(conn);
    }
    else if(httpRequest_->isPostMethod())
    {
        handlePostMethod(conn);
    }
    else
    {
        handleErrorMethod(conn);
    }

    if(!httpRequest_->isKeepAlive() || !httpRequest_->isValid())
        conn->shutdownWrite();
}

/* 在数据库中查找匹配问题 */
vector<Question> HttpServer::queryQuestion(const string& keyWords)
{
    vector<string> words;
    jieba_->CutForSearch(keyWords, words, true);

    unordered_map<string, int> idMap;
    unordered_map<string, string> queryMap;
    for(string &word : words)
    {
        LOG_DEBUG << word;
        if(word.empty() || isStopWord(word)) { LOG_DEBUG << "is stopword"; continue; }
        queryMap["word"] = word;
        DataBase::TableInfoMap wordInfo = 
            dataBase_->queryFromTable(WORD_TABLE_NAME, queryMap);
        if(wordInfo.empty()) { continue; }
        vector<string> ids = StringUtil::split(wordInfo["questionIds"], ',');
        std::for_each(ids.begin(), 
                      ids.end(), 
                      [&idMap](const string& id) { ++idMap[id]; });
    }

    /* XXX: 避免排序？*/
    vector<pair<string, int>> sortedIds(idMap.begin(), idMap.end());
    std::sort(sortedIds.begin(), sortedIds.end(), ComByValue());

    vector<Question> questionList;
    for(auto &p : sortedIds)
    {
        queryMap.clear();
        queryMap["questionId"] = p.first;
        DataBase::TableInfoMap questionInfo = 
            dataBase_->queryFromTable(QUESTION_TABLE_NAME, queryMap);
        if(questionInfo.empty()) { continue; }
        questionList.emplace_back(Question(StringUtil::toInt(questionInfo["questionId"]),     
                                           questionInfo["question"], questionInfo["questionDetail"], 
                                           questionInfo["date"], questionInfo["userId"],  
                                           questionInfo["answerIds"]));
    }
    LOG_DEBUG << questionList.size();
    return questionList;
}

/* 根据答案id从数据库中查找答案 */
vector<Answer> HttpServer::queryAnswer(const string& answerIds)
{
    vector<Answer> answerList;
    vector<string> ids = StringUtil::split(answerIds, ',');
    unordered_map<string, string> queryMap;
    for(string& id : ids)
    {
        queryMap["answerId"] = id;
        DataBase::TableInfoMap answerInfo = 
            dataBase_->queryFromTable(ANSWER_TABLE_NAME, queryMap);
        if(answerInfo.empty()) { continue; }
        answerList.emplace_back(Answer(StringUtil::toInt(answerInfo["answerId"]), 
                                       answerInfo["answer"], answerInfo["date"],     
                                       answerInfo["userId"], answerInfo["commentIds"]));
    }
    return answerList;
}

/* 根据评论id从数据库中查找评论 */
vector<Comment> HttpServer::queryComment(const string& commentIds)
{
    vector<Comment> commentList;
    vector<string> ids = StringUtil::split(commentIds, ',');
    unordered_map<string, string> queryMap;
    for(string& id : ids)
    {
        queryMap["commentId"] = id;
        DataBase::TableInfoMap commentInfo = 
            dataBase_->queryFromTable(COMMENT_TABLE_NAME, queryMap);
        if(commentInfo.empty()) { continue; }
        commentList.emplace_back(Comment(StringUtil::toInt(commentInfo["commentId"]),
                                         commentInfo["comment"], commentInfo["date"], 
                                         commentInfo["userId"]));
    }
    return commentList;
}

string HttpServer::currentTime()
{
    time_t curTime = time(nullptr); 
    struct tm curtm = *localtime(&curTime);
    string date = StringUtil::toString(curtm.tm_year) + "-"
                + StringUtil::toString(curtm.tm_mon) + "-"
                + StringUtil::toString(curtm.tm_mday) + ":"
                + StringUtil::toString(curtm.tm_hour) + "::"
                + StringUtil::toString(curtm.tm_min);
    return date;
}
bool HttpServer::insertQuestion(const string& question, const string& questionDetail, int userId)
{
    int id = dataBase_->nextQuestionId(1);
    Question questionObj(id, question, questionDetail, HttpServer::currentTime(), StringUtil::toString(userId), "");
    dataBase_->insertQuestion(questionObj, jieba_, stopWord_);
    return true;
}

bool HttpServer::insertAnswer(int questionId, const string& answer, int userId)
{
    int id = dataBase_->nextAnswerId(1);
    Answer answerObj(id, answer, HttpServer::currentTime(), StringUtil::toString(userId), "");
    dataBase_->insertAnswer(answerObj);
    dataBase_->updateQuestion(Question(questionId, "", "", "", "", StringUtil::toString(id)));
    return true;
}

bool HttpServer::insertComment(int answerId, const string& comment, int userId)
{
    return true; 
}

bool HttpServer::insertUser(const string& account, const string& password, const string& username)
{
    int id = dataBase_->nextUserId(1);
    dataBase_->insertUser(User(id, account, password, username));
    return true;
}

bool HttpServer::checkUser(const string& account)
{
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("account", account));
    DataBase::TableInfoMap userInfo = dataBase_->queryFromTable(USER_TABLE_NAME, queryMap);
    if(!userInfo.empty())
        return true;
    else
        return false;
}

HttpServer::DataInfoMap
HttpServer::loginUser(const string& account, const string& password)
{
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("account", account));
    queryMap.insert(std::make_pair("password", password));
    return dataBase_->queryFromTable(USER_TABLE_NAME, queryMap);
}


void HttpServer::handlePostMethod(const TcpConnectionPtr& conn)
{
    vector<unordered_map<string, string>> queryList;
    unordered_map<string, string> argumentsMap = httpRequest_->getArguments();

    if(httpRequest_->isQueryQuestion())
    {
        LOG_DEBUG << "isQueryQuestion";
        vector<Question> questionList = queryQuestion(argumentsMap["question"]);
        std::for_each(questionList.begin(),
                      questionList.end(),
                      [&queryList](Question& question) { queryList.emplace_back(question.toMap()); });
        httpResponse_->setCode(200);
    }
    else if(httpRequest_->isQueryAnswer())
    {
        vector<Answer> answerList = queryAnswer(argumentsMap["answerIds"]);
        std::for_each(answerList.begin(),
                      answerList.end(),
                      [&queryList](Answer& answer) { queryList.emplace_back(answer.toMap()); });
        httpResponse_->setCode(200);
    }
    else if(httpRequest_->isQueryComment())
    {
        vector<Comment> commentList = queryComment(argumentsMap["commentIds"]);
        std::for_each(commentList.begin(),
                      commentList.end(),
                      [&queryList](Comment& comment) { queryList.emplace_back(comment.toMap()); });
        httpResponse_->setCode(200);
    }
    else if(httpRequest_->isQueryUser())
    {
        httpResponse_->setCode(200);
    }
    /* 发布问题 */
    else if(httpRequest_->isInsertQuestion())
    {
        DataInfoMap insertRes;
        if(insertQuestion(argumentsMap["question"], 
                          argumentsMap["questionDetail"], 
                          StringUtil::toInt(argumentsMap["userId"])))
            insertRes.insert(std::make_pair("result", "success"));
        else
            insertRes.insert(std::make_pair("result", "failure"));
        queryList.emplace_back(insertRes);
        httpResponse_->setCode(200);
    }
    /* 回答问题 */
    else if(httpRequest_->isInsertAnswer())
    {
        insertAnswer(StringUtil::toInt(argumentsMap["questionId"]), 
                     argumentsMap["answer"],
                     StringUtil::toInt(argumentsMap["userId"]));
    }
    /* 添加评论 */
    else if(httpRequest_->isInsertComment())
    {
        insertComment(StringUtil::toInt(argumentsMap["answerId"]), 
                      argumentsMap["comment"], 
                      StringUtil::toInt(argumentsMap["userId"]));
    }
    /* 注册账号 */
    else if(httpRequest_->isInsertUser())
    {
        DataInfoMap insertRes;
        if(insertUser(argumentsMap["account"], 
                      argumentsMap["password"], 
                      argumentsMap["username"]))
            insertRes.insert(std::make_pair("result", "success"));
        else
            insertRes.insert(std::make_pair("result", "failure"));
        queryList.emplace_back(insertRes);
        httpResponse_->setCode(200);
    }
    /* 核查用户名是否已使用 */
    else if(httpRequest_->isCheckUser())
    {
        DataInfoMap checkRes;
        if(checkUser(argumentsMap["account"]))
            checkRes.insert(std::make_pair("result", "success"));
        else
            checkRes.insert(std::make_pair("result", "failure"));
        queryList.emplace_back(checkRes);
        httpResponse_->setCode(200);
    }
    /* 用户登录，检查账号密码 */
    else if(httpRequest_->isLoginUser())
    {
        DataInfoMap loginRes = loginUser(argumentsMap["account"], argumentsMap["password"]);
        if(!loginRes.empty())
            loginRes.insert(std::make_pair("result", "success"));
        else
            loginRes.insert(std::make_pair("result", "failure"));
        queryList.emplace_back(loginRes);
        httpResponse_->setCode(200);
    }
    else
    {
        handleErrorMethod(conn);
    }

    httpResponse_->setVersion(httpRequest_->version());
    /* XXX: 设置一个setBody()函数？ */
    string response = httpResponse_->generateResponse(queryList);
    LOG_DEBUG << response;
    conn->send(response);
}

void HttpServer::handleGetMethod(const TcpConnectionPtr& conn)
{
    vector<unordered_map<string, string>> queryList;
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("welcome", "hello, welcome to use app"));
    queryList.emplace_back(queryMap);
    httpResponse_->setCode(200);
    httpResponse_->setVersion(httpRequest_->version());
    string response = httpResponse_->generateResponse(queryList);
    conn->send(response);
}

void HttpServer::handleErrorMethod(const TcpConnectionPtr& conn)
{
    vector<unordered_map<string, string>> errorList;
    unordered_map<string, string> errorMap;
    errorList.emplace_back(std::move(errorMap));
    httpResponse_->setCode(404);
    httpResponse_->setVersion(httpRequest_->version());
    string response = httpResponse_->generateResponse(errorList);
    LOG_DEBUG << response;
    conn->send(response);
}
