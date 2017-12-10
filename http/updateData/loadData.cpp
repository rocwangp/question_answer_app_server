#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <regex>
#include <mysql/mysql.h>
#include "../../cppjieba/Jieba.hpp"
static const char* const DICT_PATH = "../../cppjieba/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../../cppjieba/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../../cppjieba/dict/user.dict.utf8";
static const char* const IDF_PATH = "../../cppjieba/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../../cppjieba/dict/stop_words.utf8";
using namespace std;
string toString(int n)
{
    stringstream oss;
    oss << n;
    return oss.str();
}
vector<string> split(const string& s, const char& key)
{
    vector<string> splitVector;
    size_t front_idx = 0;
    size_t back_idx = 0;
    while(front_idx != string::npos)
    {
        back_idx = s.find_first_of(key, front_idx);
        if(back_idx == string::npos)
        {
            splitVector.emplace_back(s.substr(front_idx));
            break;
        }
        splitVector.emplace_back(s.substr(front_idx, back_idx - front_idx));
        front_idx = back_idx + 1;
    }
    return splitVector;
}

string parseQuestion(ifstream &fin)
{
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
    return question;
}

string parseQuestionDetail(ifstream &fin)
{
    string questionDetail("");
    string line("");
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
    return questionDetail;
}

vector<string> parseAnswers(ifstream &fin)
{
    vector<string> answerList;
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
            answer = answer.substr(0, answer.size() - 1);
            answerList.emplace_back(answer);
            answer = "";
        }
        else
        {
            answer += line + "\n";
        }
    }   
    return answerList;
}

void filterExistAnswers(MYSQL *connPtr, vector<string>& answerList, string answerIds)
{
    vector<string> answerIdList = split(answerIds, ',');
    for(auto it = answerList.begin(); it != answerList.end();)
    {
        bool found = false;
        for(int i = 0; i < static_cast<int>(answerIdList.size()); ++i)
        {
            string findAnswerSql = "select * from answer where ";
            findAnswerSql += "answerId='" + answerIdList[i] + "' and "
                          +  "answer='" + *it + "'";
            mysql_query(connPtr, findAnswerSql.c_str());
            MYSQL_RES *result = mysql_store_result(connPtr);
            if(result)
            {
                MYSQL_ROW row = mysql_fetch_row(result);
                if(row != nullptr)
                {
                    found = true;
                    mysql_free_result(result);
                    break;
                }
                else
                {
                    mysql_free_result(result);
                }
            }
        }
        if(found)
            it = answerList.erase(it);
        else
            ++it;
    }
}

void updateQuestion(MYSQL *connPtr, string& questionId, vector<string>& answerList, string& answerIds, int answer_row)
{
    for(int i = 0; i < static_cast<int>(answerList.size()); ++i)
    {
        string insertAnswerSql = "insert into answer values(";
        insertAnswerSql += "'" + toString(answer_row + i + 1) + "'" + ","
                        +  "'" + answerList[i] + "'"  + "," 
                        +  "'" + "2017-11-15" + "'" + ","
                        +  "'" + "0" + "'" + ","
                        +  "'" + "" + "'" + ")";
        int ret = mysql_query(connPtr, insertAnswerSql.c_str());
        if(ret == -1)
            cout << "insert error" << endl;
        
        if(!answerIds.empty())
            answerIds += ",";
        answerIds += toString(answer_row + i + 1);
    }
    string updateQuestionSql = "update question set answerIds=";
    updateQuestionSql += "'" + answerIds + "'"
                      +  " where questionId='" + questionId + "'";
    mysql_query(connPtr, updateQuestionSql.c_str());
    cout << updateQuestionSql << endl;

}

void insertWord(MYSQL *connPtr, string& word, int question_row)
{
    string findSql = "select * from word where word=";
    findSql += "'" + word + "'";
    mysql_query(connPtr, findSql.c_str());
    MYSQL_RES *result = mysql_store_result(connPtr);
    if(result)
    {
        string questionIds;
        bool found = false;
        int fieldNum = mysql_num_fields(result);
        MYSQL_FIELD *field = nullptr;
        MYSQL_ROW row = mysql_fetch_row(result);
        if(row != nullptr)
        {
            for(int i = 0; i < fieldNum; ++i)
            {
                field = mysql_fetch_field_direct(result, i);
                if(strcmp(field->name, "questionIds") == 0)
                {
                    questionIds += row[i];
                    found = true;
                    break;
                }
            }
            row = mysql_fetch_row(result);

            if(!questionIds.empty())
                questionIds += ",";
            questionIds += toString(question_row + 1);
            
            string updateSql = "update word set questionIds=";
            updateSql += "'" + questionIds + "'"
                      +  " where word=" + "'" + word + "'";
            /* cout << updateSql << endl; */
            mysql_query(connPtr, updateSql.c_str());
        }
        else
        {
            string insertSql = "insert into word values(";
            insertSql += "'" + word + "'" + ","
                      +  "'" + toString(question_row) + "'" + ")";
            /* cout << insertSql << endl; */
            mysql_query(connPtr, insertSql.c_str());
        }
        mysql_free_result(result);
    }
}

string insertAnswer(MYSQL *connPtr, int answer_row, vector<string>& answerList)
{
    string answerIds("");
    for(int i = 1; i <= static_cast<int>(answerList.size()); ++i)
    {
        answerIds += toString(answer_row + i);
        string insertSql = "insert into answer values(";
        insertSql += "'" + toString(answer_row + i) + "'" + ","
                  +  "'" + answerList[i - 1] + "'" + ","
                  +  "'" + "2017-11-14" + "'" + ","
                  +  "'" + "0" + "'" + "," 
                  +  "'" + "" + "'" + ")";
        /* cout << insertSql << endl; */
        mysql_query(connPtr, insertSql.c_str());
        if(i != static_cast<int>(answerList.size()))
            answerIds += ",";
    }
    return answerIds;
}

void insertQuestion(MYSQL *connPtr, int question_row, string& question, string& questionDetail, string& answerIds)
{
    string sql = "insert into question values(";
    sql += "'" + toString(question_row + 1) + "'" + ","
        +  "'" + question + "'" + ","
        +  "'" + "2017-11-13" + "'" + ","
        +  "'" + "0" + "'" + ","
        +  "'" + answerIds + "'" + ","
        +  "'" + questionDetail + "'" + ")";
    /* cout << sql << endl; */
    mysql_query(connPtr, sql.c_str());
}

std::unordered_map<string, int>
initStopWord()
{
    std::unordered_map<string, int> stopWord;
    std::ifstream fin(STOP_WORD_PATH, std::ios_base::in);
    while(!fin.eof())
    {
        string word;
        getline(fin, word);
        stopWord[word] = 1;
    }
    return stopWord;
}

void updateDataBase()
{
    MYSQL conn;
    mysql_init(&conn);
    mysql_real_connect(&conn, "localhost", "root", "3764819", "app", 3306, NULL, 0);
    
    mysql_set_character_set(&conn, "utf8");
    string sql = "select * from question";
    mysql_query(&conn, sql.c_str());
    MYSQL_RES *result = mysql_store_result(&conn);
    int question_row = mysql_num_rows(result);
    mysql_free_result(result);

    sql = "select * from answer";
    mysql_query(&conn, sql.c_str());
    result = mysql_store_result(&conn);
    int answer_row = mysql_num_rows(result);
    mysql_free_result(result);

    cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    
    ifstream fin("zhihu_data.txt", ios_base::in);
    cout << "start update app database" << endl;
    cout << question_row << " " << answer_row << endl;

    string pattern("'");
    std::regex re(pattern);
    string fmt("\\'");
    
    std::unordered_map<string, int> stopWord = initStopWord(); 
    while(!fin.eof())
    {
        string question = regex_replace(parseQuestion(fin), re, fmt);
        string questionDetail = regex_replace(parseQuestionDetail(fin), re, fmt);
        vector<string> answerList = parseAnswers(fin); 
       
        for(string& answer : answerList)
            answer = regex_replace(answer, re, fmt);

        string findQuestionSql = "select * from question where question=";
        findQuestionSql += "'" + question + "'";
        mysql_query(&conn, findQuestionSql.c_str());
        MYSQL_RES *result = mysql_store_result(&conn);
        if(result)
        {
            string questionId(""); 
            string answerIds("");
            int fieldNum = mysql_num_fields(result);
            MYSQL_ROW row = mysql_fetch_row(result);
            MYSQL_FIELD *field = nullptr;
            if(row != nullptr)
            {
                for(int i = 0; i < fieldNum; ++i)
                {
                    field = mysql_fetch_field_direct(result, i);
                    if(strcmp(field->name, "questionId") == 0)
                        questionId = row[i];
                    else if(strcmp(field->name, "answerIds") == 0)
                        answerIds = row[i];
                }

                filterExistAnswers(&conn, answerList, answerIds);
                if(!answerList.empty())
                {
                    cout << answer_row << " " << question << " " << questionId << " " << answerIds << " " << answerList.size() << endl;
                    updateQuestion(&conn, questionId, answerList, answerIds, answer_row);
                    answer_row += answerList.size();
                }
            }
            else
            {
                vector<string> words;
                jieba.CutForSearch(question, words, true);
                for(string& word : words)
                {
                    if(word.empty() || word == "\n" || word == " " || stopWord.find(word) != stopWord.end())
                        continue;
                    insertWord(&conn, word, question_row);
                }

                string answerIds = insertAnswer(&conn, answer_row, answerList);
                cout << answer_row << " " << answerIds << endl;
                insertQuestion(&conn, question_row, question, questionDetail, answerIds);
               
                answer_row += answerList.size();
                ++question_row;
            }
            mysql_free_result(result);
        }
    }
    fin.close();
    mysql_library_end();
}

int getTableLineNum(const string& table)
{

    MYSQL conn_;
    ::mysql_init(&conn_);
    ::mysql_real_connect(&conn_, "localhost", "root", "3764819", "app", 3306, NULL, 0);
    string sql = "select * from " + table;
    mysql_query(&conn_, sql.c_str());
    MYSQL_RES *result = mysql_store_result(&conn_);
    int lineNum = mysql_num_rows(result);
    mysql_free_result(result);
    return lineNum;
}

int main()
{
    /* updateDataBase(); */
    MYSQL conn;
    ::mysql_init(&conn);
    ::mysql_real_connect(&conn, "localhost", "root", "3764819", "app", 3306, NULL, 0);
    
    ::mysql_set_character_set(&conn, "utf8");

    string sql = "select * from question where question='who are you'";
    cout << sql << endl;
    ::mysql_query(&conn, sql.c_str());
    MYSQL_RES *result = ::mysql_store_result(&conn);
    if(result)
    {
        cout << "result is not nullptr" << endl;
        MYSQL_ROW row = ::mysql_fetch_row(result);
        if(row)
        {
            cout << "row is not nullptr" << endl;
        }
        else
        {
            cout << "row is nullptr" << endl;
        }
    }
    else
    {
        cout << "result is nullptr" << endl;
    }

    ::mysql_free_result(result);
    sql = "select * from question where question='who are you aa'";
    cout << sql << endl;
    ::mysql_query(&conn, sql.c_str());
    result = ::mysql_store_result(&conn);
    if(result)
    {
        cout << "result is not nullptr" << endl;
        MYSQL_ROW row = mysql_fetch_row(result);
        if(row)
        {
            cout << "row is not nullptr" << endl;
        }
        else
        {
            cout << "row is nullptr" << endl;
        }
    }
    else
    {
        cout << "result is nullptr" << endl;
    }
    ::mysql_free_result(result);

    sql = "selectt * from question where question='能'";
    cout << sql << endl;
    ::mysql_query(&conn, sql.c_str());
    result = ::mysql_store_result(&conn);
    if(result)
    {
        cout << "result is not nullptr" << endl;
        MYSQL_ROW row = mysql_fetch_row(result);
        if(row)
        {
            cout << "row is not nullptr" << endl;
        }
        else
        {
            cout << "row is nullptr" << endl;
        }
    }
    else
    {
        cout << "result is nullptr" << endl;
    }
    ::mysql_free_result(result);
    ::mysql_library_end();
    int rows = getTableLineNum("question"); 
    cout << rows << endl;

    return 0;
}
