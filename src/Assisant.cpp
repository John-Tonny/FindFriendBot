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
#include "Assisant.h"
#include "Logger.h"
#include "Conf.h"
#include "Bot.h"
#include "MemberModel.h"
#include "Util.h"
#include "Member.h"
#include "MemberMgr.h"
#include <boost/format.hpp>

#define RESEND_TIME_BASE		1
#define RESEND_TIME_MAX			128
#define EVENT_FRIEND_ADD_TIPS	1

char* Assisant::avatar_data = nullptr;
uint32_t Assisant::avatar_data_size = 0;

Assisant::Assisant() {

}

void Assisant::tox_friend_connection_status_callback(Tox *tox, uint32_t friend_number, TOX_CONNECTION connection_status, void *user_data) {
	string pk = Util::get_pk_by_id(tox, friend_number);
	GLOBAL_LOGGER(debug) << "connection_status: " << connection_status << " pk: " << pk;
	if(TOX_CONNECTION_NONE != connection_status){
		// find tokme user for db
		Member* member = member_model.find_friend(pk);
		if (member) {
			member_mgr.add_member(member);
		}
		// update bot avatar
		std::string file_name = conf.getSave_img_path() + conf.get_avatar_filename();
		if (nullptr == avatar_data || 0 == avatar_data_size) {
			avatar_data = get_data(file_name, avatar_data_size);
			GLOBAL_LOGGER(debug) << "tokme_bot_avatar size: " << avatar_data_size;
		}
		if (avatar_data) {
			uint8_t hash_data[TOX_HASH_LENGTH] = {};
			tox_hash(hash_data, (const uint8_t*)avatar_data, avatar_data_size);
			TOX_ERR_FILE_SEND error;
			tox_file_send(tox, friend_number, TOX_FILE_KIND_AVATAR, avatar_data_size, hash_data, (const uint8_t *)file_name.c_str(), file_name.size(), &error);
			if (error != TOX_ERR_FILE_SEND_OK) {
				GLOBAL_LOGGER(error) << "tox_file_send error:" << error;
			}
		}
		
	 } else {
		member_mgr.del_member(pk);
	 }
	GLOBAL_LOGGER(info) << "online member: " << member_mgr.online_member_num();
}

void Assisant::friend_request_cb(Tox *tox, const uint8_t *public_key, const uint8_t *message, size_t length,
		void *user_data) {
	GLOBAL_LOGGER(debug) << "public_key:" << public_key;
    tox_friend_add_norequest(tox, public_key, NULL);
	update_savedata_file(tox);

	// give a tips when add friend rbobt
	// Message maybe send failed at this time, We should add a timer to resend it;
	std::string msg_res = conf.get_desc_help();
	TOX_ERR_FRIEND_BY_PUBLIC_KEY err_friend_publickey;
	uint32_t friend_number = tox_friend_by_public_key(tox, public_key, &err_friend_publickey);
	boost::replace_all(msg_res, "\\n", "\n");
	bool ret = send_friend_message_req(tox, friend_number, msg_res);	
	if (!ret) {
		uint32_t time_interval = RESEND_TIME_BASE;
		uint8_t* buf = (uint8_t*)malloc(sizeof(uint32_t));
		memset(buf, 0, sizeof(uint32_t));
		memcpy(buf, &time_interval, sizeof(uint32_t));
		tox_add_timer_event(tox, EVENT_FRIEND_ADD_TIPS, friend_number, time_interval, buf, event_timeout_cb);
	}
}

void Assisant::friend_message_cb(Tox *tox, uint32_t friend_number, TOX_MESSAGE_TYPE type,  const uint8_t *message,
		                            size_t length, void *user_data) {
	GLOBAL_LOGGER(debug) << "TOX_MESSAGE_TYPE:" << type;
	std::string msg( reinterpret_cast< char const* >(message) , length) ;
	std::string cmd;
	std::string param;
	parse_msg(msg, cmd, param);
	process_cmd(tox, friend_number, cmd, param);
}

void Assisant::file_recv_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint32_t kind, uint64_t file_size,
			                              const uint8_t *filename, size_t filename_length, void *user_data) {
	GLOBAL_LOGGER(debug) << "friend_number:" << friend_number << " file_number:" << file_number << " filename:" << filename << " filename_length:" << filename_length << " kind:" << kind ;
}

void Assisant::file_recv_chunk_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
			                                    const uint8_t *data, size_t length, void *user_data) {
	GLOBAL_LOGGER(debug) << "friend_number:" << friend_number << " file_number:" << file_number << " position:" << position;
}

void Assisant::file_chunk_request_cb(Tox *tox, uint32_t friend_number, uint32_t file_number, uint64_t position,
			                                       size_t length, void *user_data) {
	GLOBAL_LOGGER(debug) << "friend_number:" << friend_number << " file_number:" << file_number << " position:" << position << " length:" << length;
	if (!length) {
		return;
	}
	std::string file_name = conf.getSave_img_path() + conf.get_avatar_filename();
	if (nullptr == avatar_data || 0 == avatar_data_size) {
		avatar_data = get_data(file_name, avatar_data_size);
		GLOBAL_LOGGER(debug) << "tokme_bot_avatar size: " << avatar_data_size;
	}
	// maybe (position + length) >= avatar_data_size ?
	if ((position + length) > avatar_data_size) {
		GLOBAL_LOGGER(error) << "position + length is more than avatar_data_size";	
		return;
	}
	TOX_ERR_FILE_SEND_CHUNK err;
	uint8_t* byte_data = (uint8_t*)malloc(length);	
	memset(byte_data, 0, length);
	memcpy(byte_data, avatar_data + position, length);
	bool ret = tox_file_send_chunk(tox, friend_number, file_number, position, byte_data, length, &err);
	if (err !=TOX_ERR_FILE_SEND_CHUNK_OK && ret) {
		GLOBAL_LOGGER(error) << "tox_file_send_chunk error:" << err; 
	}
	free(byte_data);
}

void Assisant::update_savedata_file(const Tox *tox) {
	size_t size = tox_get_savedata_size(tox);
	uint8_t *savedata = (uint8_t *)malloc(size);
	tox_get_savedata(tox, savedata);
	FILE *f = fopen(conf.getSave_data_tmp_file().c_str(), "wb");	
	fwrite(savedata, size, 1, f);
	fclose(f);
	rename(conf.getSave_data_tmp_file().c_str(), conf.getSave_data_file().c_str());
	free(savedata);
}

char* Assisant::get_data(std::string file_name, uint32_t& file_size) {
	FILE *f = fopen(file_name.c_str(), "rb");
	uint32_t size = 0;
	char * buffer = nullptr;
	size_t result = 0;
	if (f == nullptr) {
		GLOBAL_LOGGER(error) << "File error:" << stderr;
		return buffer;
	}
	// obtain file size:
	fseek (f , 0 , SEEK_END);
	size = ftell (f);
	rewind(f);

	// allocate memory to contain the whole file:
	buffer = (char*) malloc (sizeof(char)*size);
	if (buffer == nullptr) {
		 GLOBAL_LOGGER(error) << "Memory error:" << stderr;	
		 return buffer;
	}

	// copy the file into the buffer:
	result = fread (buffer, 1, size, f);
	if (result != size) {
		GLOBAL_LOGGER(error) << "Reading error:" << stderr;
	}
	file_size = size;
	/* the whole file is now loaded in the memory buffer. */
	fclose (f);
	return buffer;
}

void Assisant::friend_name_cb(Tox *tox, uint32_t friend_number, const uint8_t *name, size_t length, void *user_data) {
	std::string pk = Util::get_pk_by_id(tox, friend_number);
	std::string tmp = (char*)name;
	// update db
	member_model.update_friend_name(pk, tmp, tox_unixtime());
	// update cache
	Member* member = member_mgr.find_member(pk);
	if (member) {
		member->set_nick_name(tmp);
	}
	// update local file
	update_savedata_file(tox);
	GLOBAL_LOGGER(debug) << "name " << tmp;  
}

void Assisant::friend_status_message_cb(Tox *tox, uint32_t friend_number, const uint8_t *message, size_t length, void *user_data) {
	std::string pk = Util::get_pk_by_id(tox, friend_number);
	std::string status_message = (char*)message;
	// update db
	member_model.update_friend_bio(pk, status_message, tox_unixtime());
	// update cache
	Member* member = member_mgr.find_member(pk);
	if (member) {
		member->set_bio(status_message);
	}
	// update local file
	update_savedata_file(tox);
	GLOBAL_LOGGER(debug) << "status message " << status_message;  
}

void Assisant::parse_msg(const std::string& msg, std::string& cmd, std::string& param) {
	size_t start = msg.find('/');
	if (start != std::string::npos) {
		size_t end = msg.find(' ');
		if (end == std::string::npos) {
			// msg have no blank space
			end = msg.size();
		}
		cmd = msg.substr(start + 1, end - start - 1); 
		param = msg.substr(end, msg.size() - end);
		GLOBAL_LOGGER(debug) << "cmd " << cmd;
		GLOBAL_LOGGER(debug) << "param " << param;
	}
}

void Assisant::process_cmd(Tox* tox, uint32_t friend_number, std::string& cmd, std::string& param) {
	bool ret = false;
	std::string msg_res = conf.get_desc_unknown_cmd();
	boost::to_lower(cmd);
	if ("start" == cmd) {
		boost::to_upper(param);
		ret = cmd_start(tox, friend_number, param, msg_res);
	} else if ("stop" == cmd) {
		ret = cmd_stop(tox, friend_number, msg_res); 	
	} else if ("set" == cmd) {
		ret = cmd_set(tox, friend_number, param, msg_res); 	
	} else if ("get" == cmd) {
		ret = cmd_get(tox, friend_number, msg_res);
	} else if ("info" == cmd) {
		ret = cmd_info(tox, friend_number, msg_res); 	
	} else if ("help" == cmd) {
		ret = cmd_help(tox, friend_number, msg_res); 	
	} else if ("readme" == cmd) {
		ret = cmd_readme(tox, friend_number, msg_res); 	
	} else {
		// invilid command
		ret = true;	
	}  
	if (ret) {
		boost::replace_all(msg_res, "\\n", "\n");
		send_friend_message_req(tox, friend_number, msg_res);	
	}
}

uint8_t* Assisant::get_name(Tox* tox, uint32_t friend_number) {
	uint8_t* name = nullptr;
	TOX_ERR_FRIEND_QUERY error;
	size_t name_size = tox_friend_get_name_size(tox, friend_number, &error);
	if (TOX_ERR_FRIEND_QUERY_OK != error) {
		GLOBAL_LOGGER(error) << "tox_friend_get_name_size error:" << error;
		return name;
	}
	name =  (uint8_t*)malloc(name_size + 1);
	memset(name, 0, name_size + 1);
	tox_friend_get_name(tox, friend_number, name, &error);
	if (TOX_ERR_FRIEND_QUERY_OK != error) {
		GLOBAL_LOGGER(error) << "tox_friend_get_name error:" << error;
		return name;
	}
	return name;
}


uint8_t* Assisant::get_status_message(Tox* tox, uint32_t friend_number) { 
	uint8_t* status_message = nullptr;
	TOX_ERR_FRIEND_QUERY error;
	size_t status_message_size = tox_friend_get_status_message_size(tox, friend_number, &error);
	if (TOX_ERR_FRIEND_QUERY_OK != error) {
		GLOBAL_LOGGER(error) << "tox_friend_get_status_message_size error:" << error;
		return status_message;
	}
	status_message = (uint8_t*)malloc(status_message_size + 1);  
	memset(status_message, 0, status_message_size + 1); 
	tox_friend_get_status_message(tox, friend_number, status_message, &error);
	if (TOX_ERR_FRIEND_QUERY_OK != error) {
		GLOBAL_LOGGER(error) << "tox_friend_get_status_message error:" << error;
		return status_message;
	}
	return status_message;
}

bool Assisant::cmd_start(Tox* tox, uint32_t friend_number, const std::string& tok_id, std::string& msg_res) {
	if (tok_id.empty()) {
		msg_res = conf.get_desc_start_non_tok();
		return true;
	}
	// get name
	uint8_t* name = get_name(tox, friend_number);
	GLOBAL_LOGGER(debug) << "cmd_start name " << (char*)name;

	// get status message
	uint8_t* status_message = get_status_message(tox, friend_number);
	GLOBAL_LOGGER(debug) << "cmd_start status_message " << (char*)status_message;
	if (!name || !status_message) {
		return false;
	}

	// save db 
	int64_t create_time = tox_unixtime();
	std::string pk = Util::get_pk_by_id(tox, friend_number);
	member_model.add_friend(tok_id, pk, (char*)name, (char*)status_message, create_time, create_time);
	Member* member = member_mgr.allocate();	
	if (member) {
		member->set_pk(pk);	
		member->set_tok_id(tok_id);	
		member->set_nick_name((char*)name);	
		member->set_bio((char*)status_message);	
	}
	member_mgr.add_member(member);
	free(name);
	free(status_message);
	msg_res = conf.get_desc_start();
	return true;		
}

bool Assisant::cmd_stop(Tox* tox, uint32_t friend_number, std::string& msg_res) {
	msg_res = conf.get_desc_stop();
	std::string pk = Util::get_pk_by_id(tox, friend_number);   
	member_mgr.del_member(pk);
	return member_model.del_friend(pk);
}

bool Assisant::cmd_set(Tox* tox, uint32_t friend_number, const std::string& signature, std::string& msg_res) {			
	if (signature.empty()) {
		msg_res = conf.get_desc_set_non_tok();
		return true;
	}
	msg_res = conf.get_desc_set();
	int64_t update_time = tox_unixtime();
	std::string pk = Util::get_pk_by_id(tox, friend_number);
	// update cache
	Member* member = member_mgr.find_member(pk);
	if (member) {
		member->set_signature(signature);
	}
	return member_model.update_friend_signature(pk, signature, update_time);
}

bool Assisant::cmd_get(Tox* tox, uint32_t friend_number, std::string& msg_res) {
	std::string pk = Util::get_pk_by_id(tox, friend_number);
	Member* member = member_mgr.random_member(pk);
	if (member) {
		msg_res = (boost::format(conf.get_desc_get()) %
				member->nick_name() %
				member->bio() %
				member->signature() % 
				member->tok_id()).str();
		GLOBAL_LOGGER(info) << member->nick_name();
	} else {
		msg_res = conf.get_desc_get_none();
		GLOBAL_LOGGER(info) << "no member can be find!"; 
	}
	return true;
}

bool Assisant::cmd_info(Tox* tox, uint32_t friend_number, std::string& msg_res) {
	std::string pk = Util::get_pk_by_id(tox, friend_number);  
	Member* member = member_mgr.find_member(pk);
	if (member) {
		msg_res = (boost::format(conf.get_desc_info()) %
				member->nick_name() %
				member->bio() %
				member->signature()).str(); 
	} else {
		uint8_t* name = get_name(tox, friend_number);
		uint8_t* status_message = get_status_message(tox, friend_number);
		if (name && status_message) {
			string name_tmp = (char*)name;
			string status_message_tmp = (char*)status_message;
			string signature_tmp = "";
			msg_res = (boost::format(conf.get_desc_info()) %
					name_tmp %
					status_message_tmp %
					signature_tmp).str(); 
			free(name);	
			free(status_message);
		}
	}
	return true;	
}

bool Assisant::cmd_help(Tox* tox, uint32_t friend_number, std::string& msg_res) {
	msg_res = conf.get_desc_help();
	return true;	
}

bool Assisant::cmd_readme(Tox* tox, uint32_t friend_number, std::string& msg_res) {
	msg_res = conf.get_desc_readme();
	return true;	
}

bool Assisant::send_friend_message_req(Tox* tox, uint32_t friend_number, const std::string& msg) {
	bool ret = false;
	TOX_ERR_FRIEND_SEND_MESSAGE errm;
	tox_friend_send_message(tox, friend_number, TOX_MESSAGE_TYPE_NORMAL, (const uint8_t*)msg.c_str(), msg.size(), &errm);
    if (TOX_ERR_FRIEND_SEND_MESSAGE_OK == errm) {
        GLOBAL_LOGGER(debug) << "ok" << " friend_nun:" << friend_number;
		ret = true;
    }
    else
    {
        GLOBAL_LOGGER(error) << "error:" << errm << " friend_nun:" << friend_number;
		ret = false;
	}
	return ret;
}

void Assisant::event_timeout_cb(Tox *tox, uint32_t friend_number, uint32_t event_type, void* user_data) {
	switch (event_type) {
		case EVENT_FRIEND_ADD_TIPS:{
			std::string msg_res = conf.get_desc_help();
			boost::replace_all(msg_res, "\\n", "\n");
			bool ret = send_friend_message_req(tox, friend_number, msg_res);	
			if (ret) {
				free(user_data);
				GLOBAL_LOGGER(debug) << "send ok, del event from timeout list";
				break;
			}
			uint32_t* tmp = (uint32_t*)(user_data);
			if (!user_data) {
				GLOBAL_LOGGER(error) << "error, user_data is null";
				break;
			}
			if (RESEND_TIME_MAX < *tmp) { 
				GLOBAL_LOGGER(debug) "event_type: "<< event_type << " RESEND_TIME_MAX";
				free(user_data); 
				break;
			}
			uint32_t time_interval = (*tmp) * 2;
			memcpy(user_data, &time_interval, sizeof(uint32_t));
			GLOBAL_LOGGER(debug) << "event_type:" << event_type << " time_interval: " << time_interval;
			tox_add_timer_event(tox, EVENT_FRIEND_ADD_TIPS, friend_number, time_interval, user_data, event_timeout_cb);
			break;						   
		}
		default:
			break;
	}	
}

Assisant::~Assisant() {
	free(avatar_data);
}
