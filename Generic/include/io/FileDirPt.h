#pragma once

#include "utility/Logger.h"

template <class Pt>
class FileDirPt{
	Pt _pt;
	std::string _path;
public:
	FileDirPt(const std::string &path , Pt pt):_pt(pt) , _path(path){
	}
	bool execute()
	{
		WIN32_FIND_DATAA FindFileData;
		std::string path = _path + "/" + "*.*";
		int finder;
		HANDLE hFind = FindFirstFileA(path.c_str(), &FindFileData);
		if(hFind == INVALID_HANDLE_VALUE)
		{
			yiqiding::utility::Logger::get("system")->log("FindFirstFileA " + path , yiqiding::utility::Logger::WARNING);
			return true;
		}

		do 
		{
			finder = FindNextFileA(hFind , &FindFileData);
			if(finder && (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE))
			{
				if(!_pt(_path + "/" + FindFileData.cFileName , FindFileData.cFileName))
				{
					yiqiding::utility::Logger::get("system")->log("MusicCloud upload "+ toString(FindFileData.cFileName) +" error");
					return false;
				}
			}
		} while(finder);

		FindClose(hFind);
		return true;
	}
};