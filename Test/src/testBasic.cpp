/**
 * Basic function testing
 * @author Shiwei Zhang
 * @date 2014.01.10
 */

#include <iostream>
#include "yiqiding/Exception.h"
#include "yiqiding/utility/Logger.h"
#include "yiqiding/io/File.h"
using namespace yiqiding;
using namespace std;

void testBasic();

int main() {
	try {
		testBasic();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	system("pause");
	return 0;
}

void testBasic() {


	
	STARTUPINFOA si = { sizeof(si) };   
	PROCESS_INFORMATION pi;   

	si.dwFlags = STARTF_USESHOWWINDOW;   
	si.wShowWindow = TRUE; //TRUE表示显示创建的进程的窗口

	CreateProcessA("..\\server.bat",
		NULL,
		NULL ,
		NULL ,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);


/*	CreateProcessA("D:\\KTV\\KTVServer\\KTVServer\\x64\\server.bat",
		NULL,
		NULL ,
		NULL ,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);*/

	/*CreateProcessA(NULL,
		"D:\\KTV\\KTVServer\\KTVServer\\x64\\server.bat",
		NULL ,
		NULL ,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);*/
	system("pause");
}
