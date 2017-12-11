#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mysql/mysql.h>

#include "../MutexLock.h"
#include "../noncopyable.h"
#include "../cppjieba/Jieba.hpp"

#include "DataInfo.h"

class DataBase : noncopyable
{
public:
    typedef std::unordered_map<std::string, int> StopWordMap;
    typedef std::unordered_map<std::string, std::string> TableInfoMap;
    typedef std::vector<TableInfoMap> TableInfoMapList;
    typedef std::vector<std::string> InsertInfoList;
    typedef std::vector<Answer> AnswerList;
    typedef std::vector<std::vector<std::string>> TableContentList;
    typedef std::shared_ptr<cppjieba::Jieba> JiebaPtr;

    DataBase(const std::string& username, 
             const std::string& password,
             const std::string& database,
             int port);
    ~DataBase();

    TableInfoMap queryFromTable(const std::string& table,
                                const TableInfoMap& queryMap);
    TableInfoMapList queryAllFromTable(const std::string& table,
                                       const TableInfoMap& queryMap);
    bool insertIntoTable(const std::string& table, 
                         const InsertInfoList& values);
    bool deleteFromTable(const std::string& table,
                         const std::string& key, 
                         const std::string& value);
    bool updateTable(const std::string& table, 
                     const TableInfoMap& updateMap,
                     const std::string& key, 
                     const std::string& value);

    bool updateDataBase(const std::string& filename, 
                        const JiebaPtr& jieba,
                        const StopWordMap& stopWord);

    int nextQuestionId(int steps = 0);
    int nextAnswerId(int steps = 0);
    int nextCommentId(int steps = 0);
    int nextUserId(int steps = 0);

    bool insertUser(const User& userObj);
    bool insertAnswer(const Answer& answerObj);
    bool insertQuestion(const Question& questionObj, const JiebaPtr& jieba, const StopWordMap& stopWord);
    bool updateQuestion(const Question& questionObj);
private:
    int getTableLineNum(const std::string& table, const std::string& columnName);
    
    void insertWord(int questionNum, const std::string& word);
    void insertAnswerList(const AnswerList& answerList);
    Question parseQuestion(std::ifstream& fin);
    /* std::string parseQuestion(std::ifstream& fin); */
    std::string parseQuestionDetail(std::ifstream& fin);
    AnswerList parseAnswers(std::ifstream& fin);

private:
    void initMySql(const std::string& username,
                   const std::string& password,
                   const std::string& database,
                   int port);;
    void initDataNum();
    void initMutex();
    
private:
    MYSQL conn_;
    int questionNum_;
    int answerNum_;
    int commentNum_;
    int userNum_;

    pthread_mutex_t mutex_;
};

