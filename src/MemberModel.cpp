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
#include "MemberModel.h"
#include "Member.h"
#include "MemberMgr.h"

MemberModel member_model;

MemberModel::MemberModel() {

}

MemberModel::~MemberModel() {

}

int MemberModel::add_friend(const std::string& tok_id,
				const std::string& pk,
                std::string nick_name,
                std::string bio,
                uint64_t create_time,
                uint64_t update_time
) {	
	int ret = 0;
    sql::Connection* connection = db_conn.allocate();
	sql::PreparedStatement *pstmt;
	try {
		pstmt = connection->prepareStatement("REPLACE INTO t_tokme_member(tok_id,pk,nick_name,bio,create_time,update_time) VALUES(?,?,?,?,?,?)");
		pstmt->setString(1, tok_id.c_str());
		pstmt->setString(2, pk.c_str());
		pstmt->setString(3, nick_name.c_str());
		pstmt->setString(4, bio.c_str());
		pstmt->setInt64(5, create_time);
		pstmt->setInt64(6, update_time);
		ret = pstmt->executeUpdate();
		pstmt->close();
		delete  pstmt;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		db_conn.release(connection);
	}
	return  ret;
}

int MemberModel::del_friend(const std::string& pk) {
	int ret = 0;
    sql::Connection* connection = db_conn.allocate();
	sql::PreparedStatement *pstmt;
	try {
		pstmt = connection->prepareStatement("DELETE FROM t_tokme_member WHERE pk = ?");
		pstmt->setString(1, pk.c_str());
		ret = pstmt->executeUpdate();
		pstmt->close();
		delete  pstmt;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		db_conn.release(connection);
	}
	return  ret;
}

int MemberModel::update_friend_signature(const std::string& pk, const std::string& signature, uint64_t update_time) {
	int ret = 0;
	sql::Connection* connection = db_conn.allocate();
	sql::PreparedStatement *pstmt;
	try {
		pstmt = connection->prepareStatement("UPDATE t_tokme_member SET signature = ?, update_time = ? WHERE pk = ?");
		pstmt->setString(1, signature.c_str());
		pstmt->setInt64(2, update_time);
		pstmt->setString(3, pk.c_str());
		ret = pstmt->executeUpdate();
		pstmt->close();
		delete  pstmt;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		db_conn.release(connection);
	}
	return  ret;
}

int MemberModel::update_friend_bio(const std::string& pk, const std::string& bio, uint64_t update_time) {
	int ret = 0;
	sql::Connection* connection = db_conn.allocate();
	sql::PreparedStatement *pstmt;
	try {
		pstmt = connection->prepareStatement("UPDATE t_tokme_member SET bio = ?, update_time = ? WHERE pk = ?");
		pstmt->setString(1, bio.c_str());
		pstmt->setInt64(2, update_time);
		pstmt->setString(3, pk.c_str());
		ret = pstmt->executeUpdate();
		pstmt->close();
		delete  pstmt;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		db_conn.release(connection);
	}
	return  ret;
}

int MemberModel::update_friend_name(const std::string& pk, const std::string& name, uint64_t update_time) {
	int ret = 0;
	sql::Connection* connection = db_conn.allocate();
	sql::PreparedStatement *pstmt;
	try {
		pstmt = connection->prepareStatement("UPDATE t_tokme_member SET nick_name = ?, update_time = ? WHERE pk = ?");
		pstmt->setString(1, name.c_str());
		pstmt->setInt64(2, update_time);
		pstmt->setString(3, pk.c_str());
		ret = pstmt->executeUpdate();
		pstmt->close();
		delete  pstmt;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		db_conn.release(connection);
	}
	return  ret;
}


Member* MemberModel::find_friend(std::string& pk) {
    sql::Connection* connection = db_conn.allocate();
    sql::PreparedStatement *pstmt;
	sql::ResultSet *res;
	Member* member = NULL;
	try {
		pstmt = connection->prepareStatement("SELECT tok_id,pk,nick_name,bio,signature FROM t_tokme_member WHERE pk = ?");
		pstmt->setString(1, pk.c_str());
		res = pstmt->executeQuery();
		if(res->next()){
			member = member_mgr.allocate();
			member->set_pk(res->getString("pk"));
			member->set_tok_id(res->getString("tok_id"));
			member->set_bio(res->getString("bio"));
			member->set_signature(res->getString("signature"));
			member->set_nick_name(res->getString("nick_name"));
		}
		pstmt->close();
		delete  pstmt;
		delete	res;
		db_conn.release(connection);
	}
	catch(sql::SQLException &e) {
		DBConn::show_sql_exception(e);
		delete  pstmt;
		delete	res;
		db_conn.release(connection);
	}
	return member;
}

