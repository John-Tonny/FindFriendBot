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
 * some operator about toxcore bot
 */
#ifndef __BOT_H
#define __BOT_H
#include <tox/tox.h>
#include <string>
using namespace std;

class Conf;

class Bot {

public:
    Bot();

    static Tox* create_tox();

    virtual ~Bot();

    static void print_debug_log(Tox *m, TOX_LOG_LEVEL level, const char *path, uint32_t line, const char *func,
                                const char *message, void *user_data);

    static const char *tox_log_level_name(TOX_LOG_LEVEL level);

    static void bootstrap(Tox *tox);

    static void set_name(Tox *tox ,string name);

    static void set_status_message(Tox *tox, string status_message);

    static void print_tox_id(Tox *tox);

    static void register_function(Tox* tox);

private:

};
#endif //__BOT_H
