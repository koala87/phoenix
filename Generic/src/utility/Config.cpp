/**
 * Configuration Manager (read only) Implementation
 * @author Shiwei Zhang
 * @date 2014.01.15
 */

#include "utility/Config.h"
#include "utility/Utility.h"
#include "Exception.h"
using namespace yiqiding::utility;
using namespace yiqiding::io;
using yiqiding::Exception;



std::string ConfigSector::getValue(const std::string &key , const std::string &default)
{
	if(count(key))
		return (*this)[key];
	return default;
}

int ConfigSector::getInt(const std::string &key , int default)
{
	if(count(key) && yiqiding::utility::isLong((*this)[key]))
		return yiqiding::utility::toInt((*this)[key]);
	return default;
}



char ConfigSector::read(istream& in) {
	char head;
	std::string line;
	size_t pos;
	std::string arg, val;

	head = in.get();
	while (!in.eof()) {
		if (head == '[')
			return head;
		else if (!isspace(head) && head != ';') {
			getline(in, line, '\n');
			line = head + line;
			pos = line.find_first_of('=');
			if (pos != std::string::npos) {
				// Parse argument and value
				arg = trim(line.substr(0,pos));
				val = trim(line.substr(pos + 1));

				// Record value
				if (val.size() > 1 && *val.begin() == '"'
					&& *val.rbegin() == '"')
					(*this)[arg] = val.substr(1,val.size()-2);
				else
					(*this)[arg] = val;
			}
		}
		else if (head == ';')
			getline(in , line , '\n');
		
		head = in.get();
	}
	return head;
}


void Config::write(const std::string & section , const std::string &name , const std::string &value)
{
	std::string sec_name = "[" + section + "]";
	std::string data = name + " = " + value;
	File		fin(_file_path);
	std::string file_content;
	std::vector<std::string> lines;
	// Open file
	fin.open(File::READ);
	getline(fin , file_content);
	fin.close();

	file_content = replace(file_content ,"\r\n" , "\n");
	lines = split(file_content , '\n');
	std::string words;

	int status = 0;
	int i ;
	for(  i = 0 ; i < lines.size() ; ++i)
	{
		if(status == 0)
		{
			words = trim(lines[i]);
			if(words == sec_name)
				status = 1;
		}
		else if(status == 1)
		{
			if(lines[i].size() > 0 && lines[i][0] == '['){	//next section
					
					int j = i - 1;
					for(; j >= 0 ; --j )
						if(lines[j] != "")
							break;
					lines.insert(lines.begin() + j + 1, data);
					break;
				}
			else{
					size_t pos = lines[i].find_first_of('=');
					if(pos != std::string::npos)
					{
						words = trim(lines[i].substr(0 , pos));
						if(words == name)
						{
							lines.erase(lines.begin() + i);
							lines.insert(lines.begin() + i  ,data);
							break;
						}
					}
				}

		}
	}
	

	if(i == lines.size()){
		if(status == 0)
		{
			if(lines[lines.size() -1]!= "")
			{
				lines.push_back("");
			}
			lines.push_back(sec_name);
			lines.push_back(data);
		}
		else if (status == 1)
		{
			int j = 0;
			for(j = lines.size() -1; j >= 0 ; --j )
				if(lines[j] != "")
					break;
			lines.insert(lines.begin() + j + 1 , data);
		}
	}


	File fp(_file_path);
	fp.open(File::CREATE | File::WRITE | File::READ);
	for(int i = 0 ; i< lines.size() ; ++i)
	{
		fp.write(lines[i].c_str() , lines[i].length());
		if(i != lines.size() -1)
		fp.write("\r\n" , 2);
	}
	fp.close();
}

void Config::load(const std::string& file_path) {
	unsigned char		head;
	std::string	sec_name;
	size_t		pos;
	File		fin(file_path);

	// Open file
	fin.open(File::READ);

	// Read file
	head = fin.get();
	while (!fin.eof()) {
		if (head == '[') {
			getline(fin, sec_name, '\n');
			pos = sec_name.find_last_of(']');
			if (pos != std::string::npos)
				sec_name.erase(pos);
			else
				throw Exception("Config", "File '" + file_path + "' corrupted", __FILE__, __LINE__);
			head = (*this)[sec_name].read(fin);
		} else if (!isspace(head) && head != ';')
			throw Exception("Config", "File '" + file_path + "' corrupted", __FILE__, __LINE__);
		else if(head == ';')
		{
			getline(fin , sec_name , '\n');
			head = fin.get();
		}
		else
			head = fin.get();
	}

	// Close file
	fin.close();
}
