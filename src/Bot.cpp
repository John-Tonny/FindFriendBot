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

#include "Bot.h"
#include <stdlib.h>
#include <sodium.h>
#include <string>
#include "Conf.h"
#include "Logger.h"
#include "Assisant.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
using namespace std;
using namespace rapidjson;

typedef struct DHT_node {
    const char *ip;
    uint16_t port;
    const char key_hex[TOX_PUBLIC_KEY_SIZE*2 + 1];
    unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
} DHT_node;

Bot::Bot() {

}

Bot::~Bot() {

}

Tox* Bot::create_tox() {
    Tox *tox;
    struct Tox_Options options;
    tox_options_default(&options);
    tox_options_set_log_callback(&options, &print_debug_log);

    FILE *f = fopen(conf.getSave_data_file().c_str(), "rb");
    if (f) {
        fseek(f, 0, SEEK_END);
        long fsize = ftell(f);
        fseek(f, 0, SEEK_SET);
        uint8_t *savedata = (uint8_t *)malloc(fsize);
        fread(savedata, fsize, 1, f);
        fclose(f);
        options.savedata_type = TOX_SAVEDATA_TYPE_TOX_SAVE;
        options.savedata_data = savedata;
        options.savedata_length = fsize;
        tox = tox_new(&options, NULL);
        free(savedata);
    } else {
        tox = tox_new(&options, NULL);
    }
    return tox;
}

void Bot::print_debug_log(Tox *m, TOX_LOG_LEVEL level, const char *path, uint32_t line, const char *func,
                               const char *message, void *user_data)
{
    if (level == TOX_LOG_LEVEL_TRACE) {
        return;
    }
    uint32_t index = user_data ? *(uint32_t *)user_data : 0;
    const char *file = strrchr(path, '/');
    file = file ? file + 1 : path;
    const int buf_size = 512;
    char buf[buf_size];
    memset(buf, 0, buf_size);
    sprintf(buf, "[#%d] %s %s:%d\t%s:\t%s\n", index, tox_log_level_name(level), file, line, func, message);
    GLOBAL_LOGGER(debug) << buf;
}

const char *Bot::tox_log_level_name(TOX_LOG_LEVEL level)
{
    switch (level) {
        case TOX_LOG_LEVEL_TRACE:
            return "TRACE";

        case TOX_LOG_LEVEL_DEBUG:
            return "DEBUG";

        case TOX_LOG_LEVEL_INFO:
            return "INFO";

        case TOX_LOG_LEVEL_WARNING:
            return "WARNING";

        case TOX_LOG_LEVEL_ERROR:
            return "ERROR";
    }
    return "<unknown>";
}

void Bot::bootstrap(Tox *tox)
{
#if 1
	uint32_t buf_size = 0;
	char* buffer = Assisant::get_data(conf.get_nodes_file_name(), buf_size);
	if (!buffer) {
		GLOBAL_LOGGER(error) << "faild at read config file:" << conf.get_nodes_file_name();
		return;
	}
	string json_buf;
	json_buf.append(buffer, 0, buf_size);
	free(buffer);
	Document d;
	d.Parse(json_buf.c_str());
	Value& tox_nodes = d["nodes"];
	vector<DHT_node> nodes;
	unsigned char key_bin[TOX_PUBLIC_KEY_SIZE];
	for (unsigned int i = 0; i < tox_nodes.Size(); ++i) {
		memset(key_bin, 0, TOX_PUBLIC_KEY_SIZE);
		const Value& tox_node = tox_nodes[i]; 
		string ipv4 = tox_node["ipv4"].GetString();
		string public_key = tox_node["public_key"].GetString();
		uint32_t port = tox_node["port"].GetInt();
        sodium_hex2bin(key_bin, TOX_PUBLIC_KEY_SIZE, public_key.c_str(), public_key.size(), NULL, NULL, NULL);
		GLOBAL_LOGGER(debug) << "ipv4:" << ipv4 << " port:" << port << " public_key:" << public_key;
		if (!tox_bootstrap(tox, ipv4.c_str(), port, key_bin, NULL)) {
			GLOBAL_LOGGER(error) << "tox_bootstrap faild";
		} 
	}
#else
    DHT_node nodes[] =
	{
		{"116.196.105.228",     33445, "F5273784AAD79F46BE7CEAE7E7E92EDCF799963DE3F4E244A19E139A1CBF911D", {0}},
		{"116.196.77.132",      33445, "040326E850DDCB49B1B2D9E3E2789D425774E4C5D783A55C09A024D05D2A8A66", {0}}
	};
    for (size_t i = 0; i < sizeof(nodes)/sizeof(DHT_node); i ++) {
        sodium_hex2bin(nodes[i].key_bin, sizeof(nodes[i].key_bin),
                       nodes[i].key_hex, sizeof(nodes[i].key_hex)-1, NULL, NULL, NULL);
        tox_bootstrap(tox, nodes[i].ip, nodes[i].port, nodes[i].key_bin, NULL);
    }
#endif
}

void Bot::set_name(Tox *tox, string name) {
    const uint8_t *pname = reinterpret_cast<const uint8_t*>(name.c_str());
    tox_self_set_name(tox, pname, name.length(), NULL);
}

void Bot::set_status_message(Tox *tox, string status_message) {
    const uint8_t *psm = reinterpret_cast<const uint8_t*>(status_message.c_str());
    tox_self_set_status_message(tox, psm, status_message.length(), NULL);
}

void Bot::print_tox_id(Tox *tox) {
    uint8_t tox_id_bin[TOX_ADDRESS_SIZE];
    tox_self_get_address(tox, tox_id_bin);
    char tox_id_hex[TOX_ADDRESS_SIZE*2 + 1];
    sodium_bin2hex(tox_id_hex, sizeof(tox_id_hex), tox_id_bin, sizeof(tox_id_bin));
    for (size_t i = 0; i < sizeof(tox_id_hex)-1; i ++) {
        tox_id_hex[i] = toupper(tox_id_hex[i]);
    }
    GLOBAL_LOGGER(info) << "Tox ID: " << tox_id_hex;
}

void Bot::register_function(Tox* tox) {
	tox_callback_friend_request(tox, Assisant::friend_request_cb);
	tox_callback_friend_message(tox, Assisant::friend_message_cb);
    tox_callback_friend_connection_status(tox, Assisant::tox_friend_connection_status_callback);
	tox_callback_friend_status_message(tox, Assisant::friend_status_message_cb);
	tox_callback_friend_name(tox, Assisant::friend_name_cb);
	tox_callback_file_recv(tox, Assisant::file_recv_cb);
	tox_callback_file_recv_chunk(tox, Assisant::file_recv_chunk_cb);
	tox_callback_file_chunk_request(tox, Assisant::file_chunk_request_cb);
}
