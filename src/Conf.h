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
 * some config item form file
 */
#ifndef __CONF_H
#define __CONF_H

#include <string>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

using namespace std;
namespace po = boost::program_options;

class Conf {

public:
    Conf();

    bool parse_option(int argc, char* argv[]);

    bool parse_conf(string file_name);

    const string &getDb_host() const;

    void setDb_host(const string &db_host);

    const string &getConf_file_name() const;

    const string &get_nodes_file_name() const;

    void setConf_file_name(const string &conf_file_name);

    int getDb_max_conn() const;

    void setDb_max_conn(int db_max_conn);

    const string &getDb_user() const;

    void setDb_user(const string &db_user);

    const string &getDb_passwd() const;

    void setDb_passwd(const string &db_passwd);

    const string &getDb_name() const;

    void setDb_name(const string &db_name);

    const string &getBot_name() const;

    void setBot_name(const string &bot_name);

    const string &getStatus_message() const;

    void setStatus_message(const string &status_message);

    const string &getSave_data_file() const;

    void setSave_data_file(const string &save_data_file);

    const string &getSave_data_tmp_file() const;

    void setSave_data_tmp_file(const string &save_data_tmp_file);

    const string &getSave_img_path() const;

    void setSave_img_path(const string &save_img_path);

    const string &getLog_file_name() const;

    void setLog_file_name(const string &log_file_name);

    const string &getLog_time_format() const;

    void setLog_time_format(const string &log_time_format);

    int getLog_file_size() const;

    void setLog_file_size(int log_file_size);

    const string &getLog_level() const;

    void setLog_level(const string &log_level);

	const string& get_desc_start() const;

	const string& get_desc_start_non_tok() const;

	const string& get_desc_stop() const;

	const string& get_desc_readme() const;

	const string& get_desc_help() const;

	const string& get_desc_set() const;

	const string& get_desc_set_non_tok() const;

	const string& get_desc_get() const;

	const string& get_desc_info() const;

	const string& get_desc_unknown_cmd() const;

	const string& get_desc_get_none() const;

	const string& get_avatar_filename() const;

    virtual ~Conf() {};

private:
	// config file 
    string conf_file_name;
	string nodes_file_name;

    //db config
    string db_host;
    int db_max_conn;
    string db_user;
    string db_passwd;
    string db_name;

    //bot config
    string bot_name;
    string status_message;
    string save_data_file;
    string save_data_tmp_file;

    //save img
    string save_img_path;
	string avatar_filename;

    // logging
	string log_file_name;
    string log_time_format;
    int log_file_size;
    string log_level;

	// tokme command desc
	string desc_start;
	string desc_start_non_tok;
	string desc_stop;
	string desc_readme;
	string desc_help;
	string desc_set;
	string desc_set_non_tok;
	string desc_get;
	string desc_info;
	string desc_unknown_cmd;
	string desc_get_none;
};

extern Conf conf;

#endif //__CONF_H
