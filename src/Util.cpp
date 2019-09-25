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
#include "Util.h"
#include "Logger.h"

Util::Util() {

}

string Util::get_pk_by_id(Tox *tox ,uint32_t friend_id) {
	uint8_t from_pk[TOX_PUBLIC_KEY_SIZE];
	char from_pk_hex[TOX_PUBLIC_KEY_SIZE*2 + 1];
	bool ret = tox_friend_get_public_key(tox, friend_id, from_pk, NULL);
	if (!ret) {
		GLOBAL_LOGGER(error) << " tox_friend_get_public_key failed ";
	}
	sodium_bin2hex(from_pk_hex, sizeof(from_pk_hex), from_pk, TOX_PUBLIC_KEY_SIZE);
	string str_fr_pk( reinterpret_cast< char const* >(from_pk_hex), TOX_PUBLIC_KEY_SIZE*2) ;
	boost::to_upper(str_fr_pk);
	return str_fr_pk;
}

int Util::get_id_by_pk(Tox *tox, string pk) {
	uint8_t bin[TOX_PUBLIC_KEY_SIZE];
	int a = sodium_hex2bin(bin,TOX_PUBLIC_KEY_SIZE,  pk.data(), pk.size(),NULL, NULL, NULL);
	TOX_ERR_FRIEND_BY_PUBLIC_KEY err_friend_publickey;
	uint32_t id = tox_friend_by_public_key(tox, bin, &err_friend_publickey);
	if(TOX_ERR_FRIEND_BY_PUBLIC_KEY_OK != err_friend_publickey){
		return -1;
	}else{
		return id;
	}
}

Util::~Util() {

}


