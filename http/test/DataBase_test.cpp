#include <iostream>
#include <string>
#include <unordered_map>

#include "../DataBase.h"
#include "../DataInfo.h"
#include "../../Logging.h"

using namespace std;

DataBase dataBase("root", "3764819", "app", 3306);

int testInsertUser()
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

void testInsertQuestion(const string& question, const string& questionDetail, int userId)
{
    int id = dataBase.nextQuestionId(1);
    Question questionObj(id, question, questionDetail, "2017-12-09", StringUtil::toString(userId), "");
    dataBase.insertQuestion(questionObj);
}

int main()
{
    int userId = testInsertUser();
    testInsertQuestion("questionbalaba")
    return 0;
}
