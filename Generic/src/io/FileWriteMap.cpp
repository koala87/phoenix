/**
 *  FileMap 
 * Lohas Network Technology Co., Ltd
 * @author Yuchun zhang
 * @date 2014.03.12
 */

#include "io/FileWriteMap.h"
#include "Exception.h"
#include "utility/Utility.h"
using yiqiding::io::FileWriteMap;
using yiqiding::io::File;


FileWriteMap::FileWriteMap():_fp(" ")
{
	_path = "";
	_length = 0;
	_curlen = 0;
	_filemap = NULL;
	_fileview = NULL;
}

void FileWriteMap::open(const std::string &path , uint32_t length)
{
	_path = path;
	_length = length;
	
	_fp.open(path , File::WRITE | File::READ);

	_filemap = CreateFileMappingA(_fp.native() ,NULL ,PAGE_READWRITE , 0 , _length , NULL );
	if(_filemap == NULL)
		throw Exception("CreateFileMappingA: " + _path, GetLastError(), __FILE__, __LINE__);

	_fileview = MapViewOfFile(_filemap , FILE_MAP_WRITE , 0 , 0 , 0);
	if (_fileview == NULL)
		throw Exception("MapViewOfFile: " + _path, GetLastError(), __FILE__, __LINE__);

}

bool FileWriteMap::Write(void *data , uint32_t length)
{

	if(_fileview == NULL || length <= 0 || data == NULL)
		return false;

	if(_curlen + length > _length)
		return false;
	
	memcpy((char *)_fileview + _curlen, data , length);
	_curlen += length;
	
	if (_curlen == _length)
	{
		FlushViewOfFile(_fileview , _length);
	}

	return true;
}

void FileWriteMap::close()
{
	if(_fileview != NULL)
	{
		UnmapViewOfFile(_fileview);
		_fileview = NULL;
	}
	if(_filemap != NULL)
	{
		CloseHandle(_filemap);
		_filemap = NULL;
	}

	_fp.close();
}


