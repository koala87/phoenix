/**
 *  FileMap 
 * Lohas Network Technology Co., Ltd
 * @author Yuchun zhang
 * @date 2014.03.12
 */

#pragma once

#include <string>
#include <cstdint>
#include "net/Socket.h"
#include "io/File.h"

namespace yiqiding { namespace io {
	class FileWriteMap{
	private:
		yiqiding::io::File   _fp;
		HANDLE _filemap;
		PVOID _fileview;
		std::string _path;
		uint32_t _length;
		uint32_t _curlen;

	public:
		FileWriteMap();
		~FileWriteMap() { close();}

		void open(const std::string& path , uint32_t length) ;
		void close();
		
		bool Write(void *data , uint32_t length);
		
		//小心操作，不能越界。
		char * getPData() const { return (char *)_fileview;}

	};



}}