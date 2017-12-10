#include <iostream>
#include <string>
#include <regex>
#include <fstream>
#include <unordered_map>
#include <memory>
#include <mysql/mysql.h>

#include <unistd.h>
#include <sys/types.h>

#include "../DataBase.h"
#include "../../cppjieba/Jieba.hpp"

static const char* const DICT_PATH = "../../cppjieba/dict/jieba.dict.utf8";
static const char* const HMM_PATH = "../../cppjieba/dict/hmm_model.utf8";
static const char* const USER_DICT_PATH = "../../cppjieba/dict/user.dict.utf8";
static const char* const IDF_PATH = "../../cppjieba/dict/idf.utf8";
static const char* const STOP_WORD_PATH = "../../cppjieba/dict/stop_words.utf8";

using namespace std;
int main()
{
    DataBase dataBase("root", "3764819", "app", 3306);
    shared_ptr<cppjieba::Jieba> jieba = std::make_shared<cppjieba::Jieba>(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH); 
    
    unordered_map<string, int> stopWordMap;
    ifstream fin(STOP_WORD_PATH, ios_base::in);
    while(!fin.eof())
    {
        string word;
        getline(fin, word);
        stopWordMap[word] = 1;
    }
    fin.close();

    dataBase.updateDataBase("zhihu_data.txt", jieba, stopWordMap);
    return 0;
}
