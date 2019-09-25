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
#ifndef __MEMBER_H__
#define __MEMBER_H__

#include <string>
using namespace std;

class Member {
public:
	static const size_t MAX_POOL_SIZE = 1024;
	typedef  int _Key;

public:
	Member(string pk, string tok_id, string nick_name, string bio, string signature);

	Member();

	~Member();

	const string pk() {return pk_;}

	void set_pk(string pk) { pk_ = pk; }

	const string tok_id() {return tok_id_;}

	void set_tok_id(string tok_id) { tok_id_ = tok_id; }

	const string nick_name() {return nick_name_;}

	void set_nick_name(string nick_name) { nick_name_ = nick_name; }

	const string bio() {return bio_;}

	void set_bio(string bio) { bio_ = bio; }

	const string signature() {return signature_;}

	void set_signature(string signature) { signature_ = signature; }

	void set_recommand_times(uint32_t recommand_times) { recommand_times_ = recommand_times; }

	void clear();

private:
	string pk_;	
	string tok_id_;
	string nick_name_;
	string bio_;
	string signature_;
	uint32_t recommand_times_;
};
#endif
