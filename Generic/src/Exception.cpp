/**
 * Exception Implementation
 * @author Shiwei Zhang
 * @date 2014.01.07
 */

#include <sstream>
#include <string>
#include <cerrno>
#include "Exception.h"
#include "utility/Utility.h"
using namespace yiqiding;
using namespace utility;

void Exception::generate_err_msg() {
	std::ostringstream sout;
	sout << _src_file << ": " << _line_no << ": ";
	if (_object != "")
		sout << _object << ": ";
	sout << _reason;
	_err_msg = sout.str();
}

Exception::Exception(const std::string& object, const std::string& reason, const std::string& src_file, line_t line_no) :
_src_file(src_file), _line_no(line_no), _object(object), _reason(reason) {
	generate_err_msg();
}

Exception::Exception(const std::string& object,	const std::string& src_file, line_t line_no) :
_src_file(src_file), _line_no(line_no), _object(object) {
	char buffer[BUFSIZ];
	strerror_s(buffer, BUFSIZ, errno); 
	_reason = buffer;
	generate_err_msg();
}

Exception::Exception(const std::string& object, DWORD code, const std::string& src_file, line_t line_no) :
_src_file(src_file), _line_no(line_no), _object(object), _reason(toString(code)) {
	generate_err_msg();
}

const char* Exception::what() const throw() {
	return _err_msg.c_str();
}

const std::string& Exception::getSrcFilename() const throw() {
	return _src_file;
}

Exception::line_t Exception::getLineNo() const throw() {
	return _line_no;
}

const std::string& Exception::getObjectName() const throw() {
	return _object;
}

const std::string& Exception::getReason() const throw() {
	return _reason;
}
