/**
 * Exception
 * @author Shiwei Zhang , Yuchun Zhang
 * @date 2014.03.14
 */

#pragma once

#include <exception>
#include <string>
#include <WinSock2.h>
#include <Windows.h>

namespace yiqiding {
	class Exception : public std::exception {
	public:
		typedef unsigned int line_t;
	private:
		std::string _src_file;
		line_t _line_no;
		std::string _object;
		std::string _reason;
		std::string _err_msg;
		void generate_err_msg();
	public:
		explicit Exception(const std::string& object, const std::string& reason, const std::string& src_file, line_t line_no);
		explicit Exception(const std::string& object, const std::string& src_file, line_t line_no);
		explicit Exception(const std::string& object, DWORD code, const std::string& src_file, line_t line_no);
		virtual ~Exception() throw() {};
		virtual const char* what() const throw();
		const std::string& getSrcFilename() const throw();
		line_t getLineNo() const throw();
		const std::string& getObjectName() const throw();
		const std::string& getReason() const throw();
	};
}
