/**
 * Packet testing
 * @author Shiwei Zhang
 * @date 2014.02.13
 */

#include <string>
#include <iostream>
#include "yiqiding/ktv/Packet.h"
#include "yiqiding/net/TCP.h"
using namespace yiqiding;
using namespace yiqiding::ktv::packet;
using namespace yiqiding::net;
using namespace std;

static const string ip = "192.168.1.79";
static const int port = 58849;

ostream& operator<<(ostream& out, Packet* pac);

void testSend();
void testReceive();
void testViewPacket();
void testTimeEchoServer();
void test_feed();
void testFeed();

int main() {
	try {
		//testSend();
		//testReceive();
		//testViewPacket();
		//testTimeEchoServer();
		//test_feed();
		testFeed();
	} catch (const exception& e) {
		cerr << "error: " << e.what() << endl;
	}
	system("pause");
	return 0;
}




void test_packet(Packet *pac , const char *data , size_t size)
{

	size_t result = 0;
	size_t index = 0;
		do{	


			result = pac->feed(data + index , size);
			index += (size - result);
			size = result;

			if (pac->isFull())
			{
				std::cout << pac->getPayload() << std::endl;
				pac = new Packet;
			}
		}while(result);
}

void test_packet2(Packet *pac , const char *data , size_t size)
{


	size_t index = 0;
	size_t result = 0;
	do{	
		index += pac->feed_origin(data + index , size - index);
		if (pac->isFull())
		{
			std::cout << pac->getPayload() << std::endl;
			pac = new Packet;
		}
	}while(index != size);

}

void test_packet1(Packet *pac , const char *data , size_t size)
{


	size_t index = 0;
	do{	
		index += pac->feed(data + index , size - index);
		if (pac->isFull())
		{
			std::cout << pac->getPayload() << std::endl;
			pac = new Packet;
		}
	}while(index != size);

}

void test_feed()
{
	Packet test;
	std::string test_content = "test";

	test.setAuth(17);
	test.setVersion(100);
	test.setPayload(test_content.c_str() , test_content.length());
	int test_len = test_content.length();
	Packet test2;

	test2.setAuth(17);
	test2.setVersion(100);
	std::string test2_content = "test2";
	test2.setPayload(test2_content.c_str() , test2_content.length());
	int test2_length = test2_content.length();

	Packet *pac = new Packet;
	

	char *tmp = new char[test2.getLength() + 24 + test.getLength()];

	//test.toNetwork();
	//test2.toNetwork();

	test_packet1(pac, test.getPayload() - 24 ,24);
	memcpy(tmp , test.getPayload() , test_len );

	memcpy(tmp + test_len , test2.getPayload() - 24 , test2_length + 24);

	test_packet1(pac , tmp , test2_length + 24 + test_len);
	




	
	

	//firtst header
	//second content + header + content



}

void testTimeEchoServer() {
	tcp::Server server;

	server.listen(15963);
	cout << "简易时间服务程序" << endl << "监听端口：" << 15963 << endl;
	while (true) {
		tcp::Client client = server.accept();
		cout << client.getAddress() << " 已连接" << endl; 
		try {
			Packet pac;
			while (!pac.isFull()) {
				char ch = client.get();
				pac.feed(&ch, 1);
			}
			cout << &pac << endl;
			std::string payload = "现在时刻: ";
			time_t rawtime = time(NULL);
			payload += ctime(&rawtime);
			client.close();
		} catch (const exception& e) {
			cerr << "error: " << e.what() << endl;
		}
		cout << client.getAddress() << " 已断开" << endl; 
	}
}

void testViewPacket() {
	tcp::Server server;
	
	server.listen(15963);
	cout << "Packet包显示服务程序" << endl << "监听端口：" << 15963 << endl;
	while (true) {
		tcp::Client client = server.accept();
		cout << client.getAddress() << " 已连接" << endl; 
		try {
			Packet pac;
			while (!pac.isFull()) {
				char ch = client.get();
				pac.feed(&ch, 1);
			}
			client.close();
			cout << &pac << endl;
		} catch (const exception& e) {
			cerr << "error: " << e.what() << endl;
		}
		cout << client.getAddress() << " 已断开" << endl; 
	}
}

void testReceive() {
	tcp::Client client;
	Packet recv_pac;
	char ch;

	client.connect(ip, port);
	cout << "connected" << endl;
	Packet(REQ_NONE).dispatch(client);
	while (!recv_pac.isFull()) {
		ch = client.get();
		cout << '.';
		recv_pac.feed(&ch, 1);
	}
	cout << endl;
	client.close();
	cout << "disconnected" << endl;

	// Printing packet
	cout << &recv_pac << endl;
}

void testSend() {
/*	string payload = "hello world";
	tcp::Client client;
	
//	Packet packet(BOX_REQ_APK_PATH);
//	packet.setPayload(payload.c_str(), payload.length());

	client.connect(ip, port);
	cout << "Connected" << endl;
	packet.dispatch(client);
	cout << "dispatched" << endl;
	client.close();*/
}

ostream& operator<<(ostream& out, Packet* pac) {
	Header header = pac->getHeader();

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
	out.write(pac->getPayload(), pac->getLength());

	return out;
}

void testFeed()
{
	char *bytes = new char[48];
	Packet *pac = new Packet();
	pac->setAuth(17);
	pac->setVersion(100);
	pac->setPayload("" , 0);	
	pac->toNetwork();
	memcpy(bytes , &pac->getHeader() , 24);
	test_packet1(new Packet() , bytes , 24);
}
