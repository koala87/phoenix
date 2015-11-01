/**
 * Utility functions
 * @author Shiwei Zhang
 * @date 2014.02.12
 */

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include "json/json.h"
#include <cstdlib>


namespace yiqiding { namespace utility {
	// Define whitespace character string.
	const std::string WHITESPACES = " \t\f\v\n\r";

	// Trim string
	std::string trim(const std::string& s);

	// Split string
	std::vector<std::string> split(const std::string& s, char delimiter);

	// replace string
	std::string replace(const std::string &src , const std::string &from , const std::string &to);

	std::string toStringLower(const std::string &src);

	std::string	getDateTime(unsigned int seconds);

	unsigned int getDateTime(const char *time);

	// Generic conversion
	template <typename T>
	std::string toString(const T& src) {
		std::ostringstream sout;
		sout << src;
		return sout.str();
	}

	std::string generateTempCode(int length = 6);


	int getdayOfMonth(int year , int month);
#ifndef NONE_JSON
	Json::Value StrToJson(const std::string &str);

	Json::Value FileToJson(const std::string &path);
#endif

	// Int type conversion
	inline int			toInt(const std::string& s)	{ return atoi(s.c_str()); };
	inline int			toInt(const char* s)		{ return atoi(s); };
	inline unsigned int	toUInt(const std::string& s){ return strtoul(s.c_str(), NULL, 0); };
	inline unsigned int	toUInt(const char* s)		{ return strtoul(s, NULL, 0); };

	// Float type conversion
	inline float	toFloat(const std::string& s)	{ return (float)atof(s.c_str()); };
	inline float	toFloat(const char* s)			{ return (float)atof(s); };
	inline double	toDouble(const std::string& s)	{ return atof(s.c_str()); };
	inline double	toDouble(const char* s)			{ return atof(s); };

	// Bool type conversion
	inline bool			toBool(const char* s)	{  if(s == NULL) return false;  return std::strcmp(s, "0") != 0; };
	inline const char*	toCString(bool b)		{ return b ? "1" : "0"; };

	// string can convert
	inline bool		isFloat(const char *s)			{	char *end; if (s == NULL) return false;	strtod(s , &end);	return (*end == NULL);}
	inline bool		isFloat(const std::string &s)	{	return isFloat(s.c_str());}

	 
	inline bool		isDouble(const std::string &s)	{	return isFloat(s);}
	inline bool		isDouble(const char *s)			{	return isFloat(s);}
	
	inline bool		isLong(const char *s)			{	char *end;if(s == NULL) return false; strtol(s , &end , 10); return (*end == NULL);}
	inline bool		isLong(const std::string &s)	{	return isLong(s.c_str());}
	

	inline bool		isULong(const char *s)			{	char *end;if(s == NULL) return false; strtoul(s , &end , 10); return (*end == NULL);}
	inline bool		isULong(const std::string &s)	{	return isULong(s.c_str());}
}}
