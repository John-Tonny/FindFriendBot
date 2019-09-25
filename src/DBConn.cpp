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
#include "DBConn.h"
#include "Logger.h"

DBConn db_conn;

DBConn::DBConn() {
    driver_ = nullptr;
    max_conn_ = 0;
}

DBConn::~DBConn() {
	for (list<sql::Connection *>::iterator __it = free_list_.begin(); __it != free_list_.end(); ++__it)
	{
		if (*__it)
		{
			_destroy(*__it);
		}
	}
}

void DBConn::init(const string& db_host, const string& db_user, const string& db_passwd, const string& db_name, uint32_t max_conn) {
    db_host_ = db_host;
    db_user_ = db_user;
    db_passwd_ = db_passwd;
    db_name_ = db_name;
    max_conn_ = max_conn;
    driver_ = get_driver_instance();
}

sql::Connection* DBConn::allocate() {
    sql::Connection* conn = nullptr;
    lock_.acquire_lock();
    if (free_list_.empty()) {
        conn = _create_connection();
    } else {

        conn = free_list_.back();
        free_list_.pop_back();
        if (conn->isClosed()) {
            delete conn;
            conn = _create_connection();
        }
    }
    lock_.release_lock();
    return conn;
}

void DBConn::release(sql::Connection* conn) {
    if (free_list_.size() > max_conn_) {
        _destroy(conn);
    } else {
        lock_.acquire_lock();
        free_list_.push_back(conn);
        lock_.release_lock();
    }
}

sql::Connection* DBConn::_create_connection() {
    sql::ConnectOptionsMap connection_properties;
    connection_properties["hostName"] = db_host_;
    connection_properties["userName"] = db_user_;
    connection_properties["password"] = db_passwd_;
    connection_properties["schema"] = db_name_;
    connection_properties["OPT_RECONNECT"] = true;
    connection_properties["OPT_CHARSET_NAME"] = "utf8mb4";
    sql::Connection * conn = nullptr;
    try
    {

        conn = driver_->connect(connection_properties);
        bool connected = conn->isValid();
        if (connected)
        {
            conn->createStatement()->execute("SET names 'utf8mb4'");
            conn->setSchema(db_name_);
            conn->setAutoCommit(1);
        }
    }
    catch (sql::SQLException &e)
    {
        show_sql_exception(e);
        delete conn;
        conn = nullptr;
    }
    return conn;
}

void DBConn::_destroy(sql::Connection* conn) {
    try {
        conn->close();
        delete conn;
        conn = nullptr;
    }
    catch (sql::SQLException &e)
    {
        show_sql_exception(e);
    }
}

void DBConn::show_sql_exception(sql::SQLException &e) {
    GLOBAL_LOGGER(error) << "# ERR: SQLException in " << __FILE__ \
        << "(" << __FUNCTION__ << ") on line " << __LINE__ \
        << "# ERR: " << e.what()
        << " (MySQL error code: " << e.getErrorCode() \
        << ", SQLState: " << e.getSQLState() << " )";
}
