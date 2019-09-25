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
#ifndef __MEMBER_MGR_H__
#define __MEMBER_MGR_H__

#include <string>
#include <map>
#include <list>
#include "Lock.h"
#include "LockedQueue.h"
#include "Member.h"

using namespace std;
using namespace tok;

class MemberMgr {

public:
	MemberMgr();

	void add_member(Member* member);
	
	Member* find_member(const string& pk);

	void del_member(const string& pk);

	Member* random_member(const string& pk);

	// number of member online 
	uint32_t online_member_num();

	Member* allocate();

	void deallcate(Member* member) { free_list.deallcate(member);}

	~MemberMgr();

private:
	void _reset(Member* member);

private:
	// key is pk, val is point to Member
	typedef map<string, Member*> map_members;
	map_members online_member;

	lock_queue<Member,mutex_lock,list<Member*> >	free_list;
};

extern MemberMgr member_mgr;
#endif
