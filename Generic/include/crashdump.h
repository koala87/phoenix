/**
 * CrashDump
 * @author Yuchun Zhang
 * @date 2014.08.15
 */
#pragma once
#include <string>
#include <WinSock2.h>
#include <Windows.h>
namespace yiqiding{

class CrashProcess;	

class CrashDump
{
private:
	
	static CrashProcess	*_process;
	//没有实现，表示不允许
	CrashDump();
	static LONG WINAPI  ExeceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo);
	
public:
	static void setCrashProcess(CrashProcess *process){ _process = process; SetUnhandledExceptionFilter(ExeceptionFilter);}
	
};

class CrashProcess
{	
public:
	virtual void process(const std::string &path) = 0;

};






}