/**
 * Logger
 * @author Shiwei Zhang
 * @date 2014.01.08
 */

#pragma once

#include <string>
#include <map>
#include <queue>
#include "Thread.h"
#include "io/File.h"

namespace yiqiding { namespace utility {

	const int MAX_LOG_LENGTH = 	1024 * 1024 * 20 ;	//20M

	class Logger : public Runnable {
	public:
		enum Level {
			NONE,
			NORMAL,
			WARNING,
			FATAL,
			
		};
	private:
		io::File _file;
		std::queue<std::string> _msg_queue;
		Mutex _msg_mutex;
		Condition _msg_cond;
		Thread _thread;
		bool _loop;
		void run();
	public:
		Logger(const std::string& path);
		~Logger();
		void log(const std::string& content, Level level = NORMAL);
		template <class T>
			void doit(const T &t){MutexGuard guard(_msg_mutex); _file.close();  t(); _file.open(yiqiding::io::File::APPEND); } 
	// static
	private:
		static Mutex _loggers_mutex;
		static std::map<Logger*, size_t> _loggers_ref_count;
		static std::map<std::string, Logger*> _loggers;
	public:
		// Thread-Safe
		static Logger* get(const std::string& name);

		// Non Thread-Safe
		static Logger* get(const std::string& name, const std::string& path);
		static Logger* alias(const std::string& name, const std::string& new_name);
		static void unload(const std::string& name);
		static void unload();
	};
}}