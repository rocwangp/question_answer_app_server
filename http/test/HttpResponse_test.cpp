#include "HttpResponse.h"
#include "HttpRequest.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

typedef unordered_map<string, string> BodyInfoMap;
typedef vector<BodyInfoMap> BodyInfoMapList;

void pushToBodyMapList(BodyInfoMap& bodyMap, BodyInfoMapList& bodyMapList, vector<string>& keys, vector<string>& values)
{
    int n = keys.size();
    for(int i = 0; i < n; ++i)
        bodyMap[keys[i]] = values[i];
    bodyMapList.emplace_back(bodyMap);
}
int main()
{
    BodyInfoMapList bodyMapList;
    BodyInfoMap bodyMap;
    vector<string> keys{"questionId", "question", "answerIds", "date", "user"};
    vector<string> values{"1", "1234567890", "1,2,3,4,5", "2017-11-10", "roc"};
    pushToBodyMapList(bodyMap, bodyMapList, keys, values);

    values = {"2", "9876543210", "6,7,8", "2017-11-9", "a"};
    pushToBodyMapList(bodyMap, bodyMapList, keys, values);

    values = {"3", "11111", "9,10,11", "2017-10-11", "b"};
    pushToBodyMapList(bodyMap, bodyMapList, keys, values);

    values = {"4", "222", "12", "2017-10-10", "c"};
    pushToBodyMapList(bodyMap, bodyMapList, keys, values);

    HttpResponse httpResponse;
    httpResponse.setCode(200);
    httpResponse.setVersion(1.1);
    string response = httpResponse.generateResponse(bodyMapList);

    cout << response << endl;

    size_t front_idx = response.find("\r\n\r\n");
    string argument = response.substr(front_idx + 4);

    string requestHeader = "POST / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    requestHeader += argument;
    HttpRequest httpRequest;
    httpRequest.parseHeader(requestHeader);

    unordered_map<string, string> argumentMap = httpRequest.getArguments();
    for(auto it = argumentMap.begin(); it != argumentMap.end(); ++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    return 0;
}
