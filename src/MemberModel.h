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
#ifndef __MEMBER_MODEL_H
#define __MEMBER_MODEL_H

#include "DBConn.h"
#include "Logger.h"

// class forward declare
class Member;

class MemberModel {
public:
    MemberModel();

    int add_friend(const std::string& tok_id,
			const std::string& pk,
            std::string nick_name,
            std::string bio,
            uint64_t create_time,
            uint64_t update_time
            );

	Member* find_friend(std::string& pk);

	int del_friend(const std::string& pk);

	int update_friend_signature(const std::string& pk, const std::string& signature , uint64_t update_time);

	int update_friend_bio(const std::string& pk, const std::string& bio, uint64_t update_time);

	int update_friend_name(const std::string& pk, const std::string& name, uint64_t update_time);

    virtual ~MemberModel();
};

extern MemberModel member_model;
#endif //__MEMBER_MODEL_H
