#include <string>
#include "crashdump.h"
#include "dbghelp.h"
#include <iostream>
#include "utility/Utility.h"

using namespace yiqiding;

LONG WINAPI yiqiding::CrashDump::ExeceptionFilter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	  
	bool bDumpOK = false;
	std::string strDumpFileName;
	DWORD dwProcess = GetCurrentProcessId();
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcess);
    if (hProcess != INVALID_HANDLE_VALUE)
    {
			char szPath[MAX_PATH];
			if (GetModuleFileNameA(NULL, szPath, sizeof(szPath)))
			{
  
				strDumpFileName = yiqiding::utility::toString(szPath) + "." + yiqiding::utility::toString(timeGetTime());
				strDumpFileName += ".dmp";
				HANDLE hFile = CreateFileA(strDumpFileName.c_str(), FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, NULL, NULL);
  
				if (hFile != INVALID_HANDLE_VALUE)
				{
						MINIDUMP_EXCEPTION_INFORMATION exception_information;
						exception_information.ThreadId = GetCurrentThreadId();
						exception_information.ExceptionPointers = ExceptionInfo;
						exception_information.ClientPointers = true;
						if (MiniDumpWriteDump(hProcess, dwProcess, hFile,   MiniDumpNormal, &exception_information, NULL, NULL))
						{
							bDumpOK = true; 
						}
						CloseHandle(hFile);
 
				}
 
			}
              CloseHandle(hProcess);	
        }
		
		if(bDumpOK && _process)
			_process->process(strDumpFileName);
  

        return EXCEPTION_EXECUTE_HANDLER;
}

CrashProcess * CrashDump::_process = NULL;