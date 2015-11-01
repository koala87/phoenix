/**
 * Dongle testing
 * @author Shiwei Zhang
 * @date 2014.02.19
 */

#include <iostream>
#include "yiqiding/Exception.h"
#include "yiqiding/ktv/Dongle.h"
using namespace yiqiding;
using namespace yiqiding::ktv;
using namespace std;

void testCheckUDID();
void testTime();

int main() {
	try {
		Dongle::getInstance()->setUserPassword("00000000");
		
		testTime();
		//testCheckUDID();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	system("pause");
	return 0;
}

void testTime() {
	auto dongle = Dongle::getInstance();

	time_t start, end;
	start = time(0);
	for (int i = 0; i < 100; i++)
		dongle->checkUDID(i);
	end = time(0);
	cout << (end - start) << endl;
}

void testCheckUDID() {
	auto dongle = Dongle::getInstance();
	
	cout << dongle->checkUDID(123) << endl;
}
