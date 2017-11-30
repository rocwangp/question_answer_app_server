#include "DataBase.h"
#include "StringUtil.h"
#include "../Logging.h"

#include <fstream>
#include <regex>

using std::ifstream;
using std::ofstream;
using std::string;
using std::vector;
using std::unordered_map;
using std::regex;


DataBase::DataBase(const string& username,
                   const string& password,
                   const string& database,
                   int port)
    : questionNum_(0),
      answerNum_(0),
      commentNum_(0),
      userNum_(0)
{

    initMySql(username, password, database, port);
    initDataNum();
}

DataBase::~DataBase()
{
    ::mysql_close(&conn_);
    ::mysql_library_end();
    ::pthread_mutex_destroy(&mutex_);
}


void DataBase::initMySql(const string& username,
                         const string& password,
                         const string& database,
                         int port)
{
    ::mysql_init(&conn_);
    if(::mysql_real_connect(&conn_, "localhost", username.c_str(), password.c_str(),
                          database.c_str(), port, NULL, 0) == nullptr)
    {
       LOG_FATAL << "connect database " << database 
                 << " with username: " << username
                 << " password: " << password << " error";
       ::abort();
    }
    else
    {
       LOG_INFO << "connect database " << database 
                << " with username: " << username
                << " password: " << password << " success";
       ::mysql_set_character_set(&conn_, "utf8");
    }
}

void DataBase::initDataNum()
{
    questionNum_ = getTableLineNum("question");
    answerNum_ = getTableLineNum("answer");
    commentNum_ = getTableLineNum("comment");
    userNum_ = getTableLineNum("user");
}

void DataBase::initMutex()
{
    ::pthread_mutex_init(&mutex_, nullptr);
}


DataBase::TableInfoMap
DataBase::queryFromTable(const string& table, const TableInfoMap& queryMap)
{
    TableInfoMap queryInfo;

    string sql = "select * from " + table + " where ";
    for(auto it = queryMap.begin(); it != queryMap.end();)
    {
        sql += it->first + "=" + "'" + it->second + "'";
        if(++it != queryMap.end()) { sql += "and"; }
    }
    LOG_DEBUG << "sql is " << sql;

    ::mysql_query(&conn_, sql.c_str());
    MYSQL_RES *result = ::mysql_store_result(&conn_);
    /* 查询出错 */
    if(!result) { return queryInfo; }
    
    int fields = ::mysql_num_fields(result);
    MYSQL_FIELD *field = nullptr;
    MYSQL_ROW row = ::mysql_fetch_row(result);
    if(row != nullptr)
    {
        for(int i = 0; i < fields; ++i)
        {
            field = ::mysql_fetch_field_direct(result, i);
            /* 查询结果存在map中 */
            queryInfo[field->name] = StringUtil::toString(row[i]); 
        }
        row = ::mysql_fetch_row(result);
    }
    return queryInfo;
}

bool DataBase::insertIntoTable(const string& table, const InsertInfoList& values)
{
    string sql = "insert into " + table + " value(";
    for(auto it = values.begin(); it != values.end();)
    {
        sql += "'" + *it + "'";
        if(++it != values.end()) { sql += ","; }
    }
    sql += ")";
    LOG_DEBUG << "insert sql is " << sql;
    ::mysql_query(&conn_, sql.c_str());
    return true;
}


bool DataBase::deleteFromTable(const string& table, const string& key, const string& value)
{
    string sql = "delete from " + table + " where " + key + " = " + "'"  + value + "'";
    LOG_DEBUG << "delete sql is " << sql;
    ::mysql_query(&conn_, sql.c_str());
    return true;
}


bool DataBase::updateTable(const string& table,
                           const TableInfoMap& updateMap,
                           const string& key, const string& value)
{
    string sql = "update " + table + " set ";
    for(auto it = updateMap.begin(); it != updateMap.end();)
    {
        sql += it->first + " = " + "'" + it->second + "'";
        if(++it != updateMap.end()) { sql += ","; }
    }
    sql += " where " + key + " = " + "'" + value + "'";
    LOG_DEBUG << "update sql is " << sql;
    ::mysql_query(&conn_, sql.c_str());
    return true;
}

int DataBase::getTableLineNum(const string& table)
{
    string sql = "select * from " + table;
    ::mysql_query(&conn_, sql.c_str());
    MYSQL_RES *result = ::mysql_store_result(&conn_);
    int lineNum = -1;
    if(result)
        lineNum = ::mysql_num_rows(result);
    ::mysql_free_result(result);
    return lineNum;
}

Question DataBase::parseQuestion(ifstream& fin)
{
    string pattern("'");
    regex re(pattern);
    string fmt("\"");

    string question("");
    string line("");
    while(!fin.eof())
    {
        getline(fin, line);
        if(line == "\r")
        {
            question = question.substr(0, question.size() - 1);
            break;
        }
        else
        {
            question += line + "\n";
        }
    }
    string questionDetail("");
    while(!fin.eof())
    {
        getline(fin, line);
        if(line == "\r")
        {
            break;
        }
        else
        {
            questionDetail += line + "\n";
        }
    }
    question = std::regex_replace(question, re, fmt);
    questionDetail = std::regex_replace(questionDetail, re, fmt);
    Question questionObj(0, question, questionDetail, "2017-11-10-00:00", "0", "");
    return questionObj;
}

DataBase::AnswerList DataBase::parseAnswers(ifstream &fin)
{

    string pattern("'");
    regex re(pattern);
    string fmt("\"");

    vector<Answer> answerList;
    string answer("");
    string line("");
    while(!fin.eof())
    {
        getline(fin, line);
        
        if(line == "\r\r")
        {
            break;     
        }
        else if(line == "\r")
        {
            answer = std::regex_replace(answer.substr(0, answer.size() - 1), re, fmt);
            try
            {
            if(!answer.empty())
                answerList.emplace_back(Answer(0, answer, "2017-11-10:00:00", "0", ""));
            }
            catch(...)
            {
                LOG_FATAL << answer;
                ::abort();
            }
            answer = "";
        }
        else
        {
            answer += line + "\n";
        }
    }   
    return answerList;
}



void DataBase::insertWord(int questionNum, const string& word)
{
    string findSql = "select * from word where word='" + word + "'";
    ::mysql_query(&conn_, findSql.c_str());
    MYSQL_RES *result = mysql_store_result(&conn_);
    if(result)
    {
        string questionIdStr("");
        int fieldNum = ::mysql_num_fields(result);
        MYSQL_ROW row = ::mysql_fetch_row(result);
        if(row != nullptr)
        {
            for(int i = 0; i < fieldNum; ++i)
            {
                MYSQL_FIELD *field = ::mysql_fetch_field_direct(result, i);
                if(::strncmp(field->name, "questionIds", ::strlen(field->name)) == 0)
                {
                    questionIdStr.assign(row[i], ::strlen(row[i]));
                    break;
                }
            }

            if(!questionIdStr.empty())
                questionIdStr += ",";
            questionIdStr += StringUtil::toString(questionNum + 1);
            
            string updateSql = "update word set ";
            updateSql += "questionIds='" + questionIdStr + "' "
                      +  "where "
                      +  "word='" + word + "'";
            ::mysql_query(&conn_, updateSql.c_str());
        }
        else
        {
            string insertSql = "insert into word values(";
            insertSql += "'" + word + "'" + ","
                      +  "'" + StringUtil::toString(questionNum) + "'" + ")";
            ::mysql_query(&conn_, insertSql.c_str());
        }
        ::mysql_free_result(result);
    }
}

bool DataBase::insertAnswer(const Answer& answer)
{
    string insertSql = "insert into answer values(";
    insertSql += "'" + StringUtil::toString(answer.answerId()) + "'" + ","
              +  "'" + answer.answer()          + "'" + ","
              +  "'" + answer.date()            + "'" + ","
              +  "'" + answer.userId()          + "'" + "," 
              +  "'" + answer.commentIdStr()    + "'" + ")";
    ::mysql_query(&conn_, insertSql.c_str());
    return true;
}

void DataBase::insertAnswerList(const AnswerList& answerObjList)
{
    std::for_each(answerObjList.begin(),
                  answerObjList.end(),
                  [this](const Answer& answerObj) { this->insertAnswer(answerObj); });
}

bool DataBase::insertQuestion(const Question& questionObj)
{
    string sql = "insert into question values(";
    sql += "'" + StringUtil::toString(questionObj.questionId()) + "'" + ","
        +  "'" + questionObj.question()         + "'" + ","
        +  "'" + questionObj.questionDetail()   + "'" + "," 
        +  "'" + questionObj.date()             + "'" + ","
        +  "'" + questionObj.userId()           + "'" + ","
        +  "'" + questionObj.answerIds()        + "'" + ")";
    ::mysql_query(&conn_, sql.c_str());

    LOG_INFO << sql;
    LOG_INFO << "insert question : " << questionObj.question();
    LOG_INFO << "answerIds : " << questionObj.answerIds();
    return true;
}


bool DataBase::updateQuestion(const Question& questionObj)
{
    string sql = "select * from question where questionId='" 
               + StringUtil::toString(questionObj.questionId()) + "'";
    ::mysql_query(&conn_, sql.c_str());
    MYSQL_RES *result = ::mysql_store_result(&conn_);
    if(result)
    {
        MYSQL_ROW row = mysql_fetch_row(result);
        int fieldNum = mysql_num_fields(result);
        string answerIdStr("");
        for(int i = 0; i < fieldNum; ++i)
        {
            MYSQL_FIELD *field = mysql_fetch_field_direct(result, i);
            if(::strncmp(field->name, "answerIds", ::strlen(field->name)) == 0)
            {
                answerIdStr.assign(row[i], strlen(row[i]));
                break;
            }
        }
        mysql_free_result(result);
    
        if(!answerIdStr.empty())
            answerIdStr.append(',', 1);
        answerIdStr.append(questionObj.answerIds());

        string updateSql = "update question set answerIds='" + answerIdStr 
                        + "' where questionId='" 
                        + StringUtil::toString(questionObj.questionId()) + "'";

        ::mysql_query(&conn_, updateSql.c_str());
        return true;
    }
    else
    {
        return false;
    }
}


bool DataBase::updateDataBase(const string& filename, 
                              const JiebaPtr& jieba,
                              const StopWordMap& stopWord)
{
    ifstream fin(filename.c_str(), std::ios_base::in);

    string pattern("'");
    regex re(pattern);
    string fmt("\"");
    
    LOG_INFO << "start update database";
    while(!fin.eof())
    {
        Question questionObj = parseQuestion(fin);
        AnswerList answerObjList = parseAnswers(fin);
        if(!questionObj.isValid() || answerObjList.empty()) { continue; }

        string findQuestionSql = "select * from question where question='" + questionObj.question() + "'";
        ::mysql_query(&conn_, findQuestionSql.c_str());
        MYSQL_RES *result = ::mysql_store_result(&conn_);
        if(result)
        {
            /* 如果该问题已存在，直接丢掉，因为主体在用户使用阶段，而不是爬取数据阶段 */
            if(::mysql_fetch_row(result) == nullptr)
            {
                /* next*Id(steps)返回当前question/answer num，同时将当前num加上steps */
                int questionId = nextQuestionId(1);
                int answerId = nextQuestionId(answerObjList.size());

                vector<string> words;
                jieba->CutForSearch(questionObj.question(), words, true);
                for(string& word : words)
                {
                    if(word.empty() || word == "\n" || word == " " || stopWord.find(word) != stopWord.end())
                        continue;
                    insertWord(questionId, word);
                }

                insertAnswerList(answerObjList);

                string answerIdStr("");
                for(int i = 0; i < static_cast<int>(answerObjList.size()); ++i)
                    answerIdStr .append(StringUtil::toString(answerId + i) + ",");
                answerIdStr.pop_back();
                questionObj.setAnswerIds(answerIdStr);
                questionObj.setQuestionId(questionId);
                insertQuestion(questionObj);
            }
            ::mysql_free_result(result);
        }
        /* 查询出错 */
        else
        {
            LOG_ERROR << "sql is incorrent";
        }

    }
    fin.close();
     
    LOG_INFO << "done";
    /* 清空文件 */
    ofstream fout(filename.c_str(), std::ios_base::out);
    fout.close();
    return true;
}



int DataBase::nextQuestionId(int steps)
{
    MutexLock(mutex_);
    int questionId = questionNum_;
    questionNum_ += steps;
    return questionId;
}

int DataBase::nextAnswerId(int steps)
{
    MutexLock(mutex_);
    int answerId = answerNum_;
    answerNum_ += steps;
    return answerId;
}

int DataBase::nextCommentId(int steps)
{
    MutexLock(mutex_);
    int commentId = commentNum_;
    commentNum_ += steps;
    return commentId;
}

int DataBase::nextUserId(int steps)
{
    MutexLock(mutex_);
    int userId = userNum_;
    userNum_ += steps;
    return userId;
}
