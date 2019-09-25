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
 * base one boost logger, make some params configurable
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/attributes/mutable_constant.hpp> 

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expressions = boost::log::expressions;
namespace trivial = boost::log::trivial;
namespace attrs = boost::log::attributes; 

static const std::string str_severity_level[] =
{
    "trace",
	"debug",
	"info",
	"warning",
	"error",
	"fatal"
};

#define GLOBAL_LOGGER(severity) \
	    BOOST_LOG_TRIVIAL(severity) << "[" << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << "] "

class Logger
{
	public:
	    Logger(const std::string& file_name = "logs/sample_%N.log",
				const std::string& timestamp_format = "%Y-%m-%d %H:%M:%S,%f",
				int rotation_size = 10, // M Bytes
				const std::string& level = "info");
        ~Logger() {};

		void init();

	private:
        Logger(const Logger&);
		Logger& operator=(const Logger&);

		std::string file_name_;
		std::string timestamp_format_;
		int rotation_size_;
		std::string level_;

		typedef std::map<std::string, logging::trivial::severity_level> type_map_severity;
		type_map_severity map_severity_;
};

#endif
