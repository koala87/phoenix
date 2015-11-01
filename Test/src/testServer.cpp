/**
 * Server testing
 * @author Shiwei Zhang
 * @date 2014.02.19
 */

#include <sstream>
#include <iostream>
#include <exception>
#include "yiqiding/ktv/KTVServer.h"
#include "yiqiding/utility/Logger.h"
#include "yiqiding/ktv/BoxProcessor.h"
#include "yiqiding/ktv/ERPProcessor.h"
#include "yiqiding/utility/Utility.h"
#include "yiqiding/utility/Transcode.h"
#ifndef NO_DONGLE
#include "yiqiding/ktv/Dongle.h"
#endif
using yiqiding::utility::Logger;
using namespace yiqiding::ktv;
using namespace yiqiding;
using namespace std;

const int BoxPort = 58849;
const int ERPPort = 25377;
//const int BoxPort = 15963;
//const int ERPPort = 15987;
HANDLE stopEvent;
BOOL WINAPI consoleHandler(DWORD ctrl);

ostream& operator<<(ostream& out, Packet* pac);
void print(const std::string& title, Packet* pac);

class TestBoxProcessor : public BoxProcessor {
public:
	TestBoxProcessor(Server* server, BoxConnection* conn, Packet* pac) : BoxProcessor(server, conn, pac) {};
	void onReceivePacket() { print("Connection " + utility::toString(_conn->getID()), _pac); BoxProcessor::onReceivePacket(); }
};

class TestERPProcessor : public ERPProcessor {
public:
	TestERPProcessor(Server* server, ERPConnection* conn, Packet* pac) : ERPProcessor(server, conn, pac) {};
	void onReceivePacket() { print("Connection " + utility::toString(_conn->getID()), _pac); ERPProcessor::onReceivePacket(); }
};

class TestServer : public ktv::Server {
public:
	TestServer() : ktv::Server(2,2,10,10) {}

	void onReceivePacket(KTVConnection* conn, Packet* pac) {
		std::auto_ptr<Packet> pack(pac);
		std::auto_ptr<packet::Processor> processor;
		switch (conn->getType()) {
		case CONNECTION_BOX:
			processor.reset(new TestBoxProcessor(this, (BoxConnection*)conn, pack.release()));
			break;
		case CONNECTION_ERP:
			processor.reset(new TestERPProcessor(this, (ERPConnection*)conn, pack.release()));
			break;
		default:
			throw Exception("Server", "Packet from unknown connection type", __FILE__, __LINE__);
		}
		_thread_pool.add(new SelfDestruct(processor.release()));
	}
};

void testBox();

int main() {
	try {
#ifndef NO_DONGLE
		Dongle::getInstance()->setUserPassword("00000000");
#endif
		testBox();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	system("pause");
	return 0;
}

void testBox() {
	Logger::get("server", "CONOUT$");
	Logger::alias("server", "system");
	Logger::alias("server", "node");
	Logger::alias("server", "stdout");

	stopEvent = CreateEvent( 0, FALSE, FALSE, 0 );
	SetConsoleCtrlHandler(consoleHandler, TRUE);
	TestServer server;

	server.setDatabaseLogin("localhost", "yiqiding", "ktv_dev", "yiqiding_ktv");

	server.setAPKPath("http://www.example.com/");
	server.getBalancer()->addNode(1, "192.168.1.233");

	server.listenBox(BoxPort);
	cout << "listening Box at " << BoxPort << endl;
	server.listenERP(ERPPort);
	cout << "listening ERP at " << ERPPort << endl;

	// Wait for the user to press CTRL-C...
	WaitForSingleObject(stopEvent, INFINITE);

	server.stop();

	// De-register console control handler: We stop the server on CTRL-C
	SetConsoleCtrlHandler(NULL, FALSE);
	CloseHandle(stopEvent);
}

BOOL WINAPI consoleHandler(DWORD ctrl) {
	switch ( ctrl ) {
	case CTRL_C_EVENT:      // Falls through..
	case CTRL_CLOSE_EVENT:
		SetEvent(stopEvent);
		return TRUE;
	default:
		return FALSE;
	}
}

ostream& operator<<(ostream& out, Packet* pac) {
	packet::Header header = pac->getHeader();

	// Printing header
	out << "---------- header -----------" << endl;
	out << "验证部分:\t" << header.auth << endl;
	out << "版本号:\t\t" << header.version << endl;
	out << "请求功能:\t" << header.request << endl;
	out << "识别码:\t\t" << header.identifier << endl;
	out << "数据大小:\t" << header.length << endl;
	out << "设备号:\t\t" << header.device_id << endl;

	// Printing payload
	out << "---------- payload ----------" << endl;
	std::string payload(pac->getPayload(), pac->getLength());
	out << utility::transcode(payload, utility::CodePage::UTF8, utility::CodePage::GBK);
	//out.write(pac->getPayload(), pac->getLength());

	return out;
}

void print(const std::string& title, Packet* pac) {
	ostringstream sout;
	sout << title << endl << pac;
	std::string out = sout.str();
	Logger::get("stdout")->log(out.c_str());
}
