#include "io/KtvDoCmd.h"
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include "assert.h"
#include "utility/Utility.h"
#include "utility/Logger.h"
using namespace yiqiding::io;


bool KTVDoCmd::doCmd(const std::string &cmd , const std::string &mode , int &code)
{
	FILE *fp = _popen(cmd.c_str() , mode.c_str());
	if(fp == NULL)
		return false;

	code = _pclose(fp);
	return true;
}

TimerofRound TimerofRound::makeTimebyday(int hour , int min , int sec)
{
	TimerofRound time = {RoundDay , hour , min , sec};
	return time;
}

TimerofRound TimerofRound::makeTimebyweek(int hour , int min , int sec , int indexofWeek)
{
	TimerofRound time = {RoundWeek , hour , min  , sec , indexofWeek};
	return time;
}

TimerofRound TimerofRound::makeTimebyMonth(int hour , int min , int sec , int indexofMonth)
{
	TimerofRound time = { RoundMonth , hour , min , sec, indexofMonth};
	return time;
}


void MysqlDataBackup::setMasterInfo(const std::string &hostname  ,const std::string &usrname , const std::string &pwd  ,
	const std::string &dbname   , int port )
{
	_master.username = usrname;
	_master.pwd = pwd;
	_master.dbname = dbname;
	_master.hostname = hostname;
	_master.port = port;
}


void MysqlDataBackup::addSlaveInfo(const std::string &hostname , const std::string &usrname  , const std::string &pwd  ,
	const std::string &dbname, int port )
{
	SQLInfo info = {usrname , pwd , dbname , hostname , port};
	_slave.push_back(info);
}

void MysqlDataBackup::operator() ()
{
	//export master
	std::ostringstream out;
	std::string fileName =  "yiqiding_ktv.sql";
	DeleteFileA(fileName.c_str());
	out << "mysqldump"<<" -u" 
		<< _master.username  << " -p"
		<< _master.pwd << " -h" 
		<< _master.hostname << " -P" 
		<< _master.port << " "
		<< _master.dbname << " " 
		<< " --single-transaction "
	    << " > "
		<< fileName;

	int code;
	bool flag = KTVDoCmd::doCmd(out.str() , "rb" ,code);
	
	if(!flag)
	{
		yiqiding::utility::Logger::get("system")->log("export do cmd " + out.str() + " error");
		return;
	}
	//import slave

	for each(auto i in _slave)
	{

		std::ostringstream out;
		out << "mysql"<<" -u" 
			<< i.username  << " -p"
			<< i.pwd << " -h" 
			<< i.hostname << " -P" 
			<< i.port << " "
			<< i.dbname 
			<< " < "
			<< fileName;
		if(!KTVDoCmd::doCmd(out.str() , "rb" ,code))
		{	
			yiqiding::utility::Logger::get("system")->log("import do cmd " + out.str() + " error");
			break;
		}
	}

	DeleteFileA(fileName.c_str());

}