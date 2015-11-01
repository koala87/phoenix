/**
 * File
 * @author Shiwei Zhang
 * @date 2014.01.13
 */

#include "io/File.h"
#include "Exception.h"
#include "utility/Utility.h"
#include "utility/Logger.h"

using namespace yiqiding::io;
using yiqiding::Exception;

#pragma warning(disable:4267)

FileHandle::FileHandle(FileHandle& __handle) {
	_handle = __handle._handle;
	__handle._handle = INVALID_HANDLE_VALUE;
}

FileHandle::~FileHandle() {
	close();
}

void FileHandle::close() {
	if (_handle != INVALID_HANDLE_VALUE)
		CloseHandle(_handle);
	_handle = INVALID_HANDLE_VALUE;
}

FileHandle& FileHandle::operator=(FileHandle& __handle) {
	if (this != &__handle) {
		close();
		_handle = __handle._handle;
		__handle._handle = INVALID_HANDLE_VALUE;
	}
	return *this;
}

//
// File
//
File::File(const std::string& __path) : _path(__path) {}

File::~File() {}

void File::open(int mode) {
	close();

	DWORD access = 0, share = FILE_SHARE_READ, flag = 0;

	if (mode & READ) {
		access |= GENERIC_READ;
		flag = OPEN_EXISTING;
	}
	if (mode & WRITE) {
		access |= GENERIC_WRITE;
		share = 0;
		flag = CREATE_NEW;
	}
	if (mode & APPEND) {
		access |= FILE_APPEND_DATA; 
		flag = OPEN_ALWAYS;
	}
	if ((mode & READ) && (mode & WRITE))
		flag = OPEN_ALWAYS;
	
	if(mode & CREATE)
		flag = CREATE_ALWAYS;

	_handle = CreateFileA(_path.c_str(), access, share, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (_handle == INVALID_HANDLE_VALUE)
		throw Exception("CreateFile: " + _path, GetLastError(), __FILE__, __LINE__);
}

void File::open(const std::string& path, int mode) {
	_path = path;
	open(mode);
}

istream& File::read(char* s, size_t n) {
	DWORD nread;
	if (!ReadFile(_handle, s, n, &nread, NULL))
		throw Exception("FileRead", GetLastError(), __FILE__, __LINE__);
	if(n != nread && nread != 0)
		yiqiding::utility::Logger::get("system")->log("read partial , read:" + yiqiding::utility::toString(nread)  + " total :" + yiqiding::utility::toString(n));
	if (nread == 0)
		ios::is_eof = true;
	return *this;
}

ostream& File::write(const char* s, size_t n) {
	DWORD nWrite;
	if (!WriteFile(_handle, s, n, &nWrite, NULL))
		throw Exception("FileWrite " + _path, GetLastError(), __FILE__, __LINE__);
	if(n != nWrite)
		yiqiding::utility::Logger::get("system")->log("write partial , write:" + yiqiding::utility::toString(nWrite)  + " total :" + yiqiding::utility::toString(n));


	return *this;
}


bool File::isExist(const std::string& path)
{

	DWORD access = 0, share = FILE_SHARE_READ, flag = 0;
	access |= GENERIC_READ;
	flag = OPEN_EXISTING;
	HANDLE handle = CreateFileA(path.c_str(), access, share, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	CloseHandle(handle);
	return true;
	
}


unsigned int File::getLength(const std::string& path)
{
	DWORD high , low;
	DWORD access = 0, share = FILE_SHARE_READ, flag = 0;
	access |= GENERIC_READ;
	flag = OPEN_EXISTING;
	HANDLE handle = CreateFileA(path.c_str(), access, share, NULL, flag, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	low = GetFileSize(handle , &high);
	CloseHandle(handle);
	return low;
}


void File::getSplitName(const std::string &path , std::string &prefix , std::string &name , std::string &suffix , char delimite)
{
	prefix = "";
	name = "";
	suffix = "";
	int first = path.find_last_of(delimite);
	if(first != -1)
	{
		prefix = path.substr(0 , first + 1);
	}
	int second = path.find_last_of('.');
	if(second > first)
	{
		name = path.substr(first + 1 , second - first - 1);
	}
	else
	{
		name = path.substr(first + 1);
	}

	if(second > first)
	suffix = path.substr(second + 1);

}