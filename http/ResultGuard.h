#pragma once

#include <mysql/mysql.h>

class ResultGuard
{
public:
    ResultGuard(MYSQL_RES *result)
        : result_(result)
    {  }

    ~ResultGuard()
    {
        ::mysql_free_result(result_); 
    }
private:
    MYSQL_RES *result_;
};
