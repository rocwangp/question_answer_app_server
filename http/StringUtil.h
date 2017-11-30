#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

namespace StringUtil
{
vector<string> split(const string& s, const char& key);
vector<string> split(const char* s, const char& key);

string toString(int n);
string toString(double n);
string toString(size_t n);
string toString(char *str);
string toString(const char* str);
string toString(string& str);

string toLower(const string& s);
int toInt(const string& s);
int toInt(const char* s);
double toDouble(const string& s);
uint64_t toUInt64(const string& s);

bool inValid(size_t n);

}
