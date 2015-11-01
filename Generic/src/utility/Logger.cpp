/**
 * Logger Implementation
 * @author Shiwei Zhang
 * @date 2014.02.13
 */

#include <ctime>
#include <sstream>
#include "Exception.h"
#include "utility/Logger.h"
using namespace yiqiding::utility;
using namespace yiqiding::io;
using yiqiding::Exception;

#pragma warning(disable:4355)	// 'this' : used in base member initializer list

yiqiding::Mutex Logger::_loggers_mutex;
std::map<Logger*, size_t> Logger::_loggers_ref_count;
std::map<std::string, Logger*> Logger::_loggers;

void Logger::run() {
	while (true) {
		std::string msg;
		DWORD	size;
		{
			MutexGuard guard(_msg_mutex);
			while (_msg_queue.empty())
				if (_loop)
					_msg_cond.wait(_msg_mutex);
				else
					return;
			msg = _msg_queue.front();
			_msg_queue.pop();
		}
		try{
			_file << msg << io::endl;
		}
		catch(const Exception &e)
		{
			yiqiding::utility::Logger::get("server")->log(e.what());
		}

		if (_file.getPath() != "CONOUT$" && 
			GetFileSize(_file.native() , &size) > MAX_LOG_LENGTH)
		{
			_file.close();
			std::ostringstream out;
			DWORD now = (DWORD)::time(NULL);
			out << _file.getPath() << "." << now << ".backup";
			
			if (rename(_file.getPath().c_str() , out.str().c_str()) != 0)
			{
				remove(out.str().c_str());
				rename(_file.getPath().c_str() , out.str().c_str());
			}
			else
			{
				_file.open(File::APPEND);
			}
	
		}

	}
}

Logger::Logger(const std::string& path) : _file(path), _thread(this), _loop(true) {
	if (path == "CONOUT$")
		_file.open(File::WRITE);
	else
		_file.open(File::APPEND);
	_thread.start();
}

Logger::~Logger() {
	_loop = false;
	_msg_cond.signal();
	_thread.join();
	_file.close();
}

void Logger::log(const std::string& content, Level level) {
	
	if(level ==  NONE)
	{
		MutexGuard guard(_msg_mutex);
		_msg_queue.push(content);
		_msg_cond.signal();
		return;
	}

	time_t now = time(NULL);
	struct tm timeinfo;
	char time_str[20];
	localtime_s(&timeinfo, &now);
	strftime(time_str,20,"%Y-%m-%d %H:%M:%S", &timeinfo);
	
	std::ostringstream sout;
	sout << "[" << time_str << "][";
	switch (level) {
	case NORMAL:
		sout << "NRML";
		break;
	case WARNING:
		sout << "WARN";
		break;
	case FATAL:
		sout << "FATL";
		break;
	}
	sout << "] " << content;
	{
		MutexGuard guard(_msg_mutex);
		_msg_queue.push(sout.str());
		_msg_cond.signal();
	}
}

Logger* Logger::get(const std::string& name) {
	MutexGuard guard(_loggers_mutex);

	std::map<std::string, Logger*>::iterator log_itr = _loggers.find(name);
	if (log_itr == _loggers.end()) {
		return get(name, name + ".log");
	} else
		return log_itr->second;
}

Logger* Logger::get(const std::string& name, const std::string& path) {
	if (_loggers.count(name))
		unload(name);

	Logger* logger = new Logger(path);
	_loggers[name] = logger;

	_loggers_ref_count[logger] = 1;

	return logger;
}

Logger* Logger::alias(const std::string& name, const std::string& new_name) {
	std::map<std::string, Logger*>::iterator log_itr = _loggers.find(name);
	if (log_itr == _loggers.end())
		throw Exception("Logger", name + "does not exist", __FILE__, __LINE__);

	Logger* logger = log_itr->second;
	_loggers_ref_count[logger]++;
	_loggers[new_name] = logger;

	return logger;
}

void Logger::unload(const std::string& name) {
	std::map<std::string, Logger*>::iterator log_itr = _loggers.find(name);
	if (log_itr != _loggers.end()) {
		std::map<Logger*, size_t>::iterator ref_itr = _loggers_ref_count.find(log_itr->second);
		_loggers.erase(log_itr);
		ref_itr->second--;
		if (ref_itr->second == 0) {
			delete ref_itr->first;
			_loggers_ref_count.erase(ref_itr);
		}
	}
}

void Logger::unload() {
	_loggers.clear();
	for (std::map<Logger*, size_t>::iterator i = _loggers_ref_count.begin(); i != _loggers_ref_count.end(); ++i)
		delete i->first;
	_loggers_ref_count.clear();
}
