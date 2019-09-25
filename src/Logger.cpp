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
#include "Logger.h"

Logger::Logger(const std::string& file_name, 
		const std::string& timestamp_format,
		int rotation_size,
		const std::string& level):
    file_name_(file_name),
	timestamp_format_(timestamp_format),
	rotation_size_(rotation_size),
	level_(level)
{
    init();
}

void Logger::init() {
	for (int i = logging::trivial::severity_level::trace; i <= logging::trivial::severity_level::fatal; ++i) {
		 map_severity_[str_severity_level[i]] = static_cast<logging::trivial::severity_level>(i);
	}
	logging::add_file_log(
		keywords::file_name = file_name_, 
		keywords::rotation_size = rotation_size_ * 1024 *1024,
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = (
			expressions::stream 
			<< "[" 
			<< expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", timestamp_format_.c_str()) << "] ["
			<< expressions::attr< boost::log::aux::process::id >("ProcessID") << "] [" 
			<< expressions::attr< boost::log::aux::thread::id >("ThreadID") << "] [" 
			<< trivial::severity << "] "
			<< expressions::message
		),
		keywords::auto_flush = true  
	);
	// map_severity_[level_] will be trace if level_ is in str_severity_level! 
	logging::core::get()->set_filter(logging::trivial::severity >= map_severity_[level_]);
	logging::add_common_attributes();

}
