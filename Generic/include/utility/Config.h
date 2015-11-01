/**
 * Configuration Manager (read only)
 * @author Shiwei Zhang
 * @date 2014.01.13
 */

#pragma once

#include <string>
#include <map>
#include "io/File.h"

namespace yiqiding { namespace utility {
	class Config;
	class ConfigSector : public std::map<std::string, std::string> {
		friend Config;
	private:
		char read(io::istream& in);
    public:
		std::string	 getValue(const std::string &key , const std::string &default);
		
		int					 getInt(const std::string &key , int default);
		

	
	};

	class Config : public std::map<std::string, ConfigSector> {
	private:
		std::string _file_path;
	
	public:


		Config() {}
		Config(const std::string& file_path) { _file_path = file_path ;load(file_path); };
		~Config() {}
		void load(const std::string& file_path);
		void write(const std::string & section , const std::string &name , const std::string &value);

	};
}}
