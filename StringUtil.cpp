#include "StringUtil.h"

#include <regex>
#include <sstream>
#include <string.h>

namespace StringUtil
{
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

vector<string> split(const char* s, const char& key)
{
    return split(toString(s), key);
}

string replaceChar(const string& s, const string& key, const string& value)
{
    std::regex re(key);
    string res = std::regex_replace(s, re, value);
    return res;
}

int toInt(const string& s)
{
    std::stringstream oss(s);
    int n;
    oss >> n;
    return n;
}

int toInt(const char* s)
{
    string str("");
    str.assign(s, ::strlen(s));
    return toInt(str);
}

double toDouble(const string& s)
{
    std::stringstream oss(s);
    double n;
    oss >> n;
    return n;
}

uint64_t toUInt64(const string& s)
{
    std::stringstream oss(s);
    uint64_t n;
    oss >> n;
    return n;
}

string toLower(const string& s)
{
    string lowerStr(s.begin(), s.end());
    for(char &ch : lowerStr)
    {
        ch = tolower(ch);
    }
    return lowerStr;
}

string toString(int n)
{
    std::stringstream oss;
    oss << n;
    return oss.str();
}

string toString(size_t n)
{
    return toString(static_cast<int>(n));
}

string toString(double n)
{
    std::stringstream oss;
    oss << n;
    return oss.str();
}

string toString(const char* str)
{
    string res;
    res.assign(str, ::strlen(str));
    return res;
}
string toString(char *str)
{
    return toString(static_cast<const char*>(str));
}

string toString(string& str)
{
    return str;
}

bool inValid(size_t n)
{
    return n != string::npos;
}
}

