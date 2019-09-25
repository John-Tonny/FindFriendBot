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
#include <MemberMgr.h>
#include "Member.h"

MemberMgr member_mgr; 

MemberMgr::MemberMgr() {	
}

void MemberMgr::add_member(Member* member) {
	online_member.insert(make_pair(member->pk(),member));
}

Member* MemberMgr::find_member(const string& pk) {
	map_members::iterator it = online_member.find(pk);	
	if (it != online_member.end()) {
		return it->second;
	}
	return nullptr;
}

void MemberMgr::del_member(const string& pk) {
	map_members::iterator it = online_member.find(pk);
	if (it != online_member.end()) {
		if (it->second) {
			deallcate(it->second);	
			it->second = nullptr;
			online_member.erase(it);
		}
	}
}

Member* MemberMgr::random_member(const string& pk) {
	// erase pk form map
	map_members online_member_tmp = online_member;		
	map_members::iterator iter = online_member_tmp.find(pk);
	if (iter != online_member_tmp.end()) {
		online_member_tmp.erase(iter);
	}
	if (!online_member_tmp.empty()) {
		map_members::iterator it = online_member_tmp.begin();
		std::advance(it, rand() % online_member_tmp.size());
		if (it != online_member_tmp.end() && it->second) {
			return it->second;	
		}
	}
	return nullptr;
}

MemberMgr::~MemberMgr() {
	for (map_members::iterator it = online_member.begin(); it != online_member.end(); ) {
		if (it->second) {
			deallcate(it->second);	
			it->second = nullptr;
			it = online_member.erase(it);
		} else {
			++it;
		}
	}
}

uint32_t MemberMgr::online_member_num() {
	return online_member.size();
}

Member* MemberMgr::allocate() {
	Member* member = free_list.allocate();	
	_reset(member);
	return member;
}

void MemberMgr::_reset(Member* member) {
	if(member) {
		member->clear();
	}	
}

