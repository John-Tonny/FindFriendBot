/****************************************************************************
MIT License

Copyright (c) 2019 TOK

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
****************************************************************************/
/*
 * mysql connector pool, it's single thread now.
 */
#ifndef __DBCONN_H
#define __DBCONN_H

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <string>
#include <list>
#include "Lock.h"

using namespace std;

class DBConn {
public:
    DBConn();

    void init(const string& db_host, const string& db_user, const string& db_passwd, const string& db_name, uint32_t max_conn);

    sql::Connection* allocate();

    void release(sql::Connection* conn);

    static void show_sql_exception(sql::SQLException &e);

    ~DBConn();

private:
    sql::Connection *_create_connection();

    void _destroy(sql::Connection* conn);

private:
    sql::Driver *driver_;

    string db_host_;
    string db_user_;
    string db_passwd_;
    string db_name_;
    uint32_t max_conn_;
    list<sql::Connection *>  free_list_;
    tok::mutex_lock lock_;
};

extern DBConn db_conn;
#endif //__DBCONN_H
