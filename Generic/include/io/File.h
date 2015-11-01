/**
 * File
 * @author Shiwei Zhang , Yuchun Zhang
 * @date 2014.03.14
 */

#pragma once

#include <WinSock2.h>
#include <Windows.h>
#include "io/iostream.h"

namespace yiqiding { namespace io {
	class FileHandle {
	protected:
		HANDLE _handle;
	public:
		FileHandle(HANDLE __handle = INVALID_HANDLE_VALUE) : _handle(__handle) {};
		FileHandle(FileHandle& __handle);	// transfer constructor
		//FileHandle(const FileHandle&);	// not implemented and never allowed
		virtual ~FileHandle();
		inline HANDLE& native() { return _handle; };
		virtual void close();
		FileHandle& operator=(FileHandle& __handle);	// transfer
	};

	class File : public FileHandle, public iostream {
	public:
		enum Mode {
			READ	= 0x1,
			WRITE	= 0x2,
			APPEND	= 0x4,
			CREATE	= 0x8,
		};
	private:
		std::string _path;
	public:
		File(const std::string& __path);
		~File();
		void open(int mode = READ | WRITE);
		void open(const std::string& path, int mode);
		istream& read(char* s, size_t n);
		ostream& write(const char* s, size_t n);

		
		static bool isExist(const std::string& path);
		static unsigned int getLength(const std::string &path);
		// Windows System use '\\' , Web and Linux use /.
		static void getSplitName(const std::string &path , std::string &prefix , std::string &name , std::string &suffix , char  delimiter = '\\');

		const std::string &getPath() {return _path;}

	};



}}
