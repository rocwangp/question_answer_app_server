#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <memory>
#include <algorithm>

#include "../DataBase.h"
#include "../DataInfo.h"
#include "../../Logging.h"
#include "../../cppjieba/Jieba.hpp"

using namespace std;

static const char* const DICT_PATH = "../../cppjieba/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../../cppjieba/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../../cppjieba/dict/user.dict.utf8";
static const char* const IDF_PATH = "../../cppjieba/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../../cppjieba/dict/stop_words.utf8";



int testInsertUser( DataBase& dataBase)
{
    int userId = dataBase.nextUserId(1);
    string account = "accountbalabala" + StringUtil::toString(userId);
    string password = "passwordbalabala" + StringUtil::toString(userId);
    string username = "namebalabala" + StringUtil::toString(userId);
    cout << "check whether the account is exist" << endl;
    while(true)
    {
        unordered_map<string, string> queryMap;
        queryMap.insert(std::make_pair("account", account));
        unordered_map<string, string> userInfo = dataBase.queryFromTable("user", queryMap);
        if(!userInfo.empty())
            cout << "there is same account, change account" << endl;
        else
        {
            cout << "there is no same account, go to insertUser" << endl;
            break;
        } 

        account += StringUtil::toString(1);
    }
    
    cout << "start insertUser" << endl;
    User userObj(userId, account, password, username);
    dataBase.insertUser(userObj);

    cout << "check whether insert successfully" << endl;
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("account", account));
    unordered_map<string, string> userInfo = dataBase.queryFromTable("user", queryMap);
    if(!userInfo.empty())
        cout << "there is same account, insert successfully" << endl;
    else
        cout << "error : there is no same account" << endl;

    cout << "start login" << endl;
    queryMap.clear();
    queryMap.insert(std::make_pair("account", account));
    queryMap.insert(std::make_pair("password", password));
    userInfo = dataBase.queryFromTable("user", queryMap);
    if(!userInfo.empty())
        cout << "user found, login successfully" << endl;
    else
        cout << "no user, login failure" << endl;

    for(auto &p : userInfo)
        cout << p.first << " : " << p.second << endl;

    return userId;
}

struct ComByValue
{
    bool operator()(pair<string, int>& lhs, pair<string, int>& rhs)
    {
        return lhs.second > rhs.second;
    }
};

Question testInsertQuestion(DataBase& dataBase, 
                            const string& question, 
                            const string& questionDetail, 
                            int userId, 
                            const shared_ptr<cppjieba::Jieba>& jieba, 
                            unordered_map<string, int>& stopWord)
{
    
    int id = dataBase.nextQuestionId(1);
    Question questionObj(id, question, questionDetail, "2017-12-09", StringUtil::toString(userId), "", "0");
    dataBase.insertQuestion(questionObj, jieba, stopWord);

    
    unordered_map<string, string> queryMap;
    unordered_map<string, int> idMap;
    vector<string> words;
    jieba->CutForSearch(question, words, true);
    for(auto &word : words)
    {
        if(stopWord.find(word) != stopWord.end())
            continue;
        
        queryMap["word"] = word;
        DataBase::TableInfoMap wordInfo = 
            dataBase.queryFromTable("word", queryMap);
        vector<string> idStrList = StringUtil::split(wordInfo["questionIds"], ',');
        std::for_each(idStrList.begin(),
                      idStrList.end(),
                      [&idMap](const string& id) { idMap[id]++; });
    }

    vector<pair<string,int>> idList(idMap.begin(), idMap.end());
    std::sort(idList.begin(), idList.end(), ComByValue());
     
    vector<Question> questionList;
    queryMap.clear();
    for(auto &p : idList)
    {
        queryMap["questionId"] = p.first;
        DataBase::TableInfoMap questionInfo = 
            dataBase.queryFromTable("question", queryMap);
        if(questionInfo.empty())
            continue;
        questionList.emplace_back(Question(StringUtil::toInt(questionInfo["questionId"]),
                            questionInfo["question"], questionInfo["questionDetail"],
                            questionInfo["date"],     questionInfo["userId"],
                            questionInfo["answerIds"],
                            questionInfo["adopted"]));
    }

    for(auto &questionObj : questionList)
    {
        cout << "questionId : " << questionObj.questionId() << endl;
        cout << "question   : " << questionObj.question() << endl;
        cout << "questionDetail : " << questionObj.questionDetail() << endl;
        cout << "date : " << questionObj.date() << endl;
        cout << "userId : " << questionObj.userId() << endl;
        cout << "answerIds : " << questionObj.answerIds() << endl;
        cout << "adopted : " << questionObj.adopted() << endl;
    }

    return questionObj;
}

void testInsertAnswer(DataBase& dataBase, const string& answer, int userId, int questionId)
{
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("questionId", StringUtil::toString(questionId)));
    DataBase::TableInfoMap questionInfo = 
        dataBase.queryFromTable("question", queryMap);
    if(questionInfo.empty())
        return ;

    cout << "insert Answer, before update Question" << endl;
    cout << "questionId : " << questionInfo["questionId"] << endl;
    cout << "question   : " << questionInfo["question"] << endl;
    cout << "questionDetail : " << questionInfo["questionDetail"] << endl;
    cout << "date : " << questionInfo["date"] << endl;
    cout << "userId : " << questionInfo["userId"] << endl;
    cout << "answerIds : " << questionInfo["answerIds"] << endl;
    cout << "adopted : " << questionInfo["adopted"] << endl;

    int id = dataBase.nextAnswerId(1);
    Answer answerObj(id, answer, "2017-12-11", StringUtil::toString(userId), "");
    dataBase.insertAnswer(answerObj);
    
    string answerIds = questionInfo["answerIds"];
    if(!answerIds.empty())
        answerIds += ",";
    answerIds += StringUtil::toString(id);

    cout << answerIds << endl;
    dataBase.updateQuestion(Question(questionId, 
                                     questionInfo["question"], 
                                     questionInfo["questionDetail"], 
                                     questionInfo["date"],
                                     questionInfo["userId"], 
                                     answerIds,
                                     questionInfo["adopted"]));
    
    questionInfo = dataBase.queryFromTable("question", queryMap);
    if(questionInfo.empty())
    {
        cout << " query question error" << endl;
        return;
    }

    cout << "insert Answer, after update Question" << endl;
    cout << "questionId : " << questionInfo["questionId"] << endl;
    cout << "question   : " << questionInfo["question"] << endl;
    cout << "questionDetail : " << questionInfo["questionDetail"] << endl;
    cout << "date : " << questionInfo["date"] << endl;
    cout << "userId : " << questionInfo["userId"] << endl;
    cout << "answerIds : " << questionInfo["answerIds"] << endl;

    queryMap.clear();
    queryMap["answerId"] = StringUtil::toString(id);
    DataBase::TableInfoMap answerInfo = dataBase.queryFromTable("answer", queryMap);
    if(answerInfo.empty())
    {
        cout << "query answer error" << endl;
        return;
    }
    
    cout << "insert Answer, after insert Answer" << endl;
    cout << "answerId : " << answerInfo["answerId"] << endl;
    cout << "answer : " << answerInfo["answer"] << endl;
    cout << "date : " << answerInfo["date"] << endl;
    cout << "userId : " << answerInfo["userId"] << endl;
    cout << "commentIds : " << answerInfo["commentIds"] << endl;
}

void testQueryNoAdoptedQuestion(DataBase& dataBase)
{
    unordered_map<string, string> queryMap;
    queryMap.insert(std::make_pair("adopted", "0"));
    DataBase::TableInfoMapList questionInfoList = 
        dataBase.queryAllFromTable("question", queryMap);

    for(auto &questionInfo : questionInfoList)
    {
        cout << "questionId : " << questionInfo["questionId"] << endl;
        cout << "question   : " << questionInfo["question"] << endl;
        cout << "questionDetail : " << questionInfo["questionDetail"] << endl;
        cout << "date : " << questionInfo["date"] << endl;
        cout << "userId : " << questionInfo["userId"] << endl;
        cout << "answerIds : " << questionInfo["answerIds"] << endl;
        cout << "adopted : " << questionInfo["adopted"] << endl;
    }
}


int main()
{
    DataBase dataBase("root", "3764819","app", 3306);
    shared_ptr<cppjieba::Jieba> jieba = std::make_shared<cppjieba::Jieba>(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
    unordered_map<string, int> stopWord;
    ifstream fin(STOP_WORD_PATH, ios_base::in);
    while(!fin.eof())
    {
        string line;
        getline(fin, line);
        stopWord[line] = 1;
    }
    fin.close();
    int userId = testInsertUser(dataBase);
    cout << endl;
    Question questionObj = testInsertQuestion(dataBase, 
                                              "questionbalaba" + StringUtil::toString(userId), 
                                              "questionDetailbalabala" + StringUtil::toString(userId), 
                                              userId, jieba, stopWord);
    cout << endl;
    userId = testInsertUser(dataBase);
    cout << endl;
    testInsertAnswer(dataBase, 
                     "answerbalabalabala" + StringUtil::toString(userId) , 
                     userId, questionObj.questionId());

    cout << endl;
    userId = testInsertUser(dataBase);
    cout << endl;
    testInsertAnswer(dataBase, 
                     "answerbalabalabala" + StringUtil::toString(userId) , 
                     userId, questionObj.questionId());

    cout << endl;
    testQueryNoAdoptedQuestion(dataBase);
    return 0;
}
