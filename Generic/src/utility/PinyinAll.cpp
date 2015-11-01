/**
 * Pinyin related functions
 * Convert Chinese¡¢Japanese¡¢Korean to Pinyin 
 * @author Yuchun Zhang
 * @date 2014.03.27
 */
#include "utility/pinyinAll.h"
#include <string>
#include "io/File.h"
#include "Exception.h"
#include <iostream>

using namespace yiqiding::utility;
using namespace yiqiding::io;
using namespace yiqiding;
PinyinAll * PinyinAll::_pinyinall = NULL;
Mutex PinyinAll::_mutex;
const std::string chidict = "./chidict";
const std::string kordict = "./kordict";
const std::string japdict = "./japdict";
 
void PinyinAll::create(const std::string &chipath , const std::string &korpath , const std::string &jappath)
{
	File chifp(chipath);
	File korfp(korpath);
	File japfp(jappath);
	chifp.open(File::READ);
	korfp.open(File::READ);
	japfp.open(File::READ);

	DWORD high,low;
	int i;
	low = GetFileSize(chifp.native() ,&high);
	if (low % sizeof(CodePinyin) != 0 || low == 0)
		throw Exception(chidict + "may be wrong!" , GetLastError() , __FILE__ , __LINE__);
	_chipnum = low / sizeof(CodePinyin);
	_chipinyin = new CodePinyin[_chipnum];
	chifp.read((char *)_chipinyin , low);
	chifp.close();
	for ( i = 0 ; i <_chipnum ; i++ )
	{
		_chipinyin[i]._code = ntohs(_chipinyin[i]._code);
	}


	low = GetFileSize(korfp.native(),&high);
	if (low % sizeof(CodePinyin) != 0 || low == 0)
		throw Exception(kordict + "may be wrong!" , GetLastError() , __FILE__ , __LINE__);
	_kornum = low / sizeof(CodePinyin);
	_korpinyin = new CodePinyin[_kornum];
	korfp.read((char *)_korpinyin , low);
	korfp.close();
	for ( i = 0 ; i < _kornum ; i++)
	{
		_korpinyin[i]._code = ntohs(_korpinyin[i]._code); 
	}


	low = GetFileSize(japfp.native(),&high);
	if (low % sizeof(CodePinyin) != 0 || low == 0)
		throw Exception(japdict + "may be wrong!" , GetLastError() , __FILE__ , __LINE__);
	_japnum = low / sizeof(CodePinyin);
	_jappinyin = new CodePinyin[_japnum];
	japfp.read((char *)_jappinyin , low);
	japfp.close();
	for ( i = 0 ; i < _japnum ; i++)
	{
		_jappinyin[i]._code = ntohs(_jappinyin[i]._code); 
	}

}

PinyinAll * PinyinAll::getInstance()
{
	if (_pinyinall == NULL)
	{
		MutexGuard _guard(_mutex);
		if(_pinyinall == NULL)
		{
			_pinyinall = new PinyinAll();
			_pinyinall->create(chidict , kordict , japdict);
		}
	}

	return _pinyinall;
}

std::string PinyinAll::bsearch(uint16_t wCode , CodeType type)
{
	std::string result;
	CodePinyin *pinyin;
	int num;
	if(type == CHN)//4E00-9FA5 
	{
		pinyin = _chipinyin;
		num    = _chipnum;
	}
	else if (type == KOR)//AC00-D7A3
	{
		pinyin = _korpinyin;
		num    = _kornum;
	}
	else if (type == JAP)
	{
		pinyin = _jappinyin;
		num = _japnum;
	}
	else
	{
		return result;
	}
	
	int max = num - 1;
	int min = 0;
	int mid;
	uint16_t value;
	while(max >= min)
	{
		mid = (min + max )/ 2;
		value = pinyin[mid]._code;
		if (wCode == value)
		{
			result = pinyin[mid]._pinyin;
			break;
		}
		else if (value < wCode)
		{
			min = mid + 1;
		}
		else 
		{
			max = mid - 1;
		}
	}
	return result;
}

std::string PinyinAll::toPinyin(const std::wstring lines , CodeType type  , bool lowletter , const std::string& split  )
{
	std::string result;
	std::string value;
	const WCHAR *start = lines.c_str();
	std::vector<WCHAR> alp;
	//num 1 0x30 0x39
	//english 2 0x41~0x5a 0x61~0x7a
	for (int i = 0 ; i < lines.length(); i++)
	{


		if(0x0020 <= start[i] && start[i] <= 0x007E)  
		{
			if( /*(0x0030 <= start[i] && start[i] <= 0x0039) ||*/
				 (0x0041 <= start[i] && start[i] <= 0x005a) ||
				(0x0061 <= start[i] &&  start[i] <= 0x007a )
				)
			alp.push_back(start[i]);
			//not blank
			else if (!alp.empty())
			{
					value.clear();
					value.assign(alp.begin() , alp.end());
					if (value != "")
					{
						if(result == "")
							result = value;
						else
							result += split + value;
					}
					alp.clear();
			}	
			continue;
		}
		
		if (!alp.empty())
		{
			value.clear();
			value.assign(alp.begin() , alp.end());
			if (value != "")
			{
				if(result == "")
					result = value;
				else
					result += split + value;
			}
			alp.clear();
		}	


		value = bsearch(start[i] , type);
		if (value != "")
		{
			if(result == "")
				result = value;
			else
				result += split + value;
		}
	}

	if(!alp.empty())
	{
		value.clear();
		value.assign(alp.begin() , alp.end());
		if (value != "")
		{
			if(result == "")
				result = value;
			else
				result += split + value;
		}
		alp.clear();
	}

	if (lowletter)
	{
		for (int i = 0 ; i < result.length(); i++)
		{
			if (result[i] >= 'A' && result[i] <= 'Z')
			{
				result[i] = result[i] + 'a' - 'A';
			}
		}
	}

	return result;
}

