/**
 * Utility functions implementation
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#include "utility/Utility.h"
#include "io/File.h"
#include <string.h>
#include <ctime>


std::string yiqiding::utility::trim(const std::string& s) {
	size_t pos = s.find_first_not_of(WHITESPACES);
	if (pos == std::string::npos)
		return "";
	else
		return s.substr(pos, s.find_last_not_of(WHITESPACES) - pos + 1);
}


int yiqiding::utility::getdayOfMonth(int year , int month)
{
	int numberOfDays = -1; 
	if (month == 4 || month == 6 || month == 9 || month == 11) 
		numberOfDays = 30; 
	else if (month == 2) 
	{ 
		bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0); 
	if (isLeapYear) 
		numberOfDays = 29; 
	else 
		numberOfDays = 28; 
	} 
	else 
		numberOfDays = 31; 
	
	return numberOfDays;
}

std::string yiqiding::utility::getDateTime(unsigned int seconds)
{
	struct tm timeinfo;
	char time_str[20];
	time_t now = seconds;
	localtime_s(&timeinfo, &now);
	strftime(time_str,20,"%Y-%m-%d %H:%M:%S", &timeinfo);
	return time_str;
}

unsigned int yiqiding::utility::getDateTime(const char *time)
{
	int number[6] = {0};
	char value[5];
	int j = 0;
	int k = 0;
	while(*time)
	{
		if('0' <= *time && *time <= '9')
		{
			value[j++] = *time;
		}
		else
		{
			value[j] = 0;
			number[k++] = atoi(value);
			j = 0;
		}
		++time;
	}

	value[j] = 0;
	number[k] = atoi(value);

	struct tm timeinfo;
	timeinfo.tm_year = number[0] - 1900;
	timeinfo.tm_mon = number[1] - 1;
	timeinfo.tm_mday = number[2] ;
	timeinfo.tm_hour = number[3];
	timeinfo.tm_min = number[4];
	timeinfo.tm_sec = number[5];
	

	return (unsigned int)mktime(&timeinfo);
}



std::vector<std::string> yiqiding::utility::split(const std::string& s, char delimiter) {
	std::vector<std::string> result;
	std::string element;

	for each (auto c in s) {
		if (c == delimiter) {
			result.push_back(element);
			element.clear();
		} else
			element.push_back(c);
	}
	result.push_back(element);

	return result;
}

std::string  yiqiding::utility::toStringLower(const std::string &src)
{
	std::string dst = src;
	for (int i = 0 ; i < src.length(); ++i)
	{
		dst[i] = tolower(src[i]);
	}
	return dst;
}

std::string yiqiding::utility::replace(const std::string &src , const std::string &from , const std::string &to)
{
	std::string::size_type index = 0;
	std::string dst;
	int fromLen = from.length();
	std::string::size_type result;
	while(true)
	{
		result = src.find(from.c_str() , index);
		if(result == std::string::npos)
			break;
		dst.append(src.c_str() + index , src.c_str() + result);
		dst += to;
		index = result + fromLen;
	}
	dst.append(src.c_str() + index , src.c_str() + src.length());
	return dst;
}

std::string yiqiding::utility::generateTempCode(int length /* = 6 */)
{
	//0~9 , a~z , A~Z	10	26	26
	std::string result;
	int i = 0 , r = 0;
	std::vector<char> value;
	while(i < length)
	{
		r = rand() % 62;
		if(r < 10)
			value.push_back('0' + r);
		else if (r < 36 )
			value.push_back('a' + r - 10);
		else
			value.push_back('A' + r - 36);

		i++;
	}
	result.clear();
	result.assign(value.begin() , value.end());
	return result;

}


#ifndef NONE_JSON

Json::Value yiqiding::utility::StrToJson(const std::string &str)
{
	Json::Value root;
	Json::Reader reader;
	if(!reader.parse(str , root))
		return NULL;
	return root;
}

Json::Value yiqiding::utility::FileToJson(const std::string &path)
{
	yiqiding::io::File fp(path);
	try{
		fp.open(yiqiding::io::File::READ);
		DWORD low = GetFileSize(fp.native() , &low);
		std::auto_ptr<char> pdata(new char[low + 1]);
		fp.read(pdata.get() , low);
		pdata.get()[low] = 0;
		fp.close();
		Json::Value root;
		Json::Reader reader;
		if(!reader.parse( pdata.get() ,root))
			return Json::nullValue;
		return root;
	}catch(...){}
	
	return Json::nullValue;
}

#endif