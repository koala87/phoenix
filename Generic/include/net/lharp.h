#pragma once
#include "Thread.h"
#include <vector>
#include <string>
#include <sstream>
#include "pcap.h"
#include <iostream>
#include <iomanip>
#include <map>

namespace yiqiding{ namespace net {
	
	class MacAddr :public std::vector < unsigned char >
	{
	public:
		MacAddr(){}

		MacAddr(const std::string & mac)
		{
			int addr[6];
			sscanf_s(mac.c_str(), "%2x:%2x:%2x:%2x:%2x:%2x"
				, &addr[0], &addr[1], &addr[2]
			, &addr[3], &addr[4], &addr[5]);
			for (size_t i = 0; i < 6; ++i)
				push_back(addr[i]);
		}

		friend std::ostream& operator<<(std::ostream& os, const MacAddr& addr) {
			for (size_t i = 0; i < addr.size(); ++i){
				os << std::setw(2) << std::setfill('0') << std::hex << (unsigned int) addr[i];
				if (i != addr.size() - 1)
					os << ":";
			}
			return os;
		}

		std::string toString() const
		{
			char address[64];
			sprintf_s(address ,"%.2X:%.2X:%.2X:%.2X:%.2X:%.2X" ,(*this)[0] ,(*this)[1],(*this)[2]
			,(*this)[3],(*this)[4],(*this)[5]);
			return address;
		}
	};

	
	// Ethernet header 
	//
	// The wire format of an Ethernet header is:
	// 0                 5                 11    13
	// +-----------------+-----------------+-----+
	// |destination mac  |source mac       |type |
	// |XX:XX:XX:XX:XX:XX|YY:YY:YY:YY:YY:YY|ZZ:ZZ|
	// +-----------------+-----------------+-----+

	class ethernet_header
	{
	public:
		ethernet_header() { std::fill(rep_, rep_ + sizeof(rep_), 0); }

		void dst(const MacAddr &mac_address) {
			for (size_t i = 0; i < mac_address.size(); ++i) {
				rep_[0 + i] = mac_address[i];
			}
		}

		void src(const MacAddr &mac_address) {
			for (size_t i = 0; i < mac_address.size(); ++i) {
				rep_[6 + i] = mac_address[i];
			}
		}

		void type(unsigned short n) { encode(12, 13, n); }

		MacAddr dst() const {
			MacAddr mac_address;
			for (int i = 0; i < 6; ++i) {
				mac_address.push_back(rep_[0 + i]);
			}
			return mac_address;
		}

		MacAddr src() const {
			MacAddr mac_address;
			for (int i = 0; i < 6; ++i) {
				mac_address.push_back(rep_[6 + i]);
			}
			return mac_address;
		}

		unsigned short type() const { return decode(12, 13); }

		friend std::istream& operator>>(std::istream& is, ethernet_header& header)
		{
			return is.read(reinterpret_cast<char*>(header.rep_), 14);
		}

		friend std::ostream& operator<<(std::ostream& os, const ethernet_header& header)
		{
			return os.write(reinterpret_cast<const char*>(header.rep_), 14);
		}

	private:
		unsigned short decode(int a, int b) const
		{
			return (rep_[a] << 8) + rep_[b];
		}

		void encode(int a, int b, unsigned short n)
		{
			rep_[a] = static_cast<unsigned char>(n >> 8);
			rep_[b] = static_cast<unsigned char>(n & 0xFF);
		}

		unsigned char rep_[14];
	};

	// ARP header
	//
	// The wire format of an ARP header is:
	// 
	// 0               8               16                             31
	// +-------------------------------+------------------------------+      ---
	// |                               |                              |       ^
	// |     Hardware type (HTYPE)     |    Protocol type (PTYPE)     |       |
	// |                               |                              |       |
	// +---------------+---------------+------------------------------+    4 bytes
	// |               |               |                              |       ^
	// |   Hard. len.  |  Proto. len.  |       Operation (OPER)       |       |
	// |    (HLEN)     |    (PLEN)     |                              |       |
	// +-------------------------------+------------------------------+    8 bytes
	// |                                                              |       ^
	// |                  Sender hardware address (SHA)               |       |
	// |                                                              |       |
	// +--------------------------------------------------------------+    14 bytes
	// |                                                              |       ^
	// |                  Sender protocol address (SPA)               |       |
	// |                                                              |       |
	// +--------------------------------------------------------------+    18 bytes
	// |                                                              |       ^
	// |                  Target hardware address (THA)               |       |
	// |                                                              |       |
	// +--------------------------------------------------------------+    24 bytes
	// |                                                              |       ^
	// |                  Target protocol address (TPA)               |       |
	// |                                                              |       |
	// +--------------------------------------------------------------+    28 bytes
	class arp_header
	{
	public:

		arp_header(){ std::fill(rep_, rep_ + sizeof(rep_), 0); }

		//setter
		void htype(unsigned short n){ encode(0, 1, n); }

		void ptype(unsigned short n){ encode(2, 3, n); }

		void hsize(unsigned char n){ rep_[4] = n; }

		void psize(unsigned char n){ rep_[5] = n; }

		void opcode(unsigned short n){ encode(6, 7, n); }

		void sha(const MacAddr & mac){
			for (size_t i = 0; i < mac.size(); ++i)
				rep_[8 + i] = mac[i];
		}

		//network address
		void spa(unsigned int address){
			unsigned char *bytes = (unsigned char *)&address;
		
			rep_[14] = bytes[3];
			rep_[15] = bytes[2];
			rep_[16] = bytes[1];
			rep_[17] = bytes[0];
		}

		void tha(const MacAddr& mac){
			for (size_t i = 0; i < mac.size(); ++i)
				rep_[18 + i] = mac[i];
		}

		//network address
		void tpa(unsigned int address){
			unsigned char *bytes = (unsigned char *)&address;
			rep_[24] = bytes[3];
			rep_[25] = bytes[2];
			rep_[26] = bytes[1];
			rep_[27] = bytes[0];
		}

		//getter
		unsigned short htype() const { return decode(0, 1); }

		unsigned short ptype() const { return decode(2, 3); }

		unsigned char hsize() const { return rep_[4]; }

		unsigned char psize() const { return rep_[5]; }

		unsigned short opcode() const { return decode(6, 7); }

		MacAddr sha()const {
			MacAddr mac;
			for (size_t i = 0; i < 6; i++)
				mac.push_back(rep_[8 + i]);
			return mac;
		}

		std::string spa() const {
			std::ostringstream sout;

			sout << int(rep_[14]) << '.'
				<< int(rep_[15]) << '.'
				<< int(rep_[16]) << '.'
				<<int(rep_[17]);

			return sout.str();
		}

		MacAddr tha()const{
			MacAddr mac;
			for (int i = 0; i < 6; ++i)
				mac.push_back(rep_[18 + i]);
			return mac;
		}

		 std::string tpa() const {
			 std::ostringstream sout;

			 sout << int(rep_[24]) << '.'
				 << int(rep_[25]) << '.'
				 << int(rep_[26]) << '.'
				 <<int(rep_[27]);

			 return sout.str();
		}
		//overloads

		friend std::istream& operator>>(std::istream& is, arp_header& header)
		{
			return is.read(reinterpret_cast<char*>(header.rep_), 28);
		}

		friend std::ostream& operator<<(std::ostream& os, const arp_header& header)
		{
			return os.write(reinterpret_cast<const char*>(header.rep_), 28);
		}

	private:
		void encode(int a, int b, unsigned short n)
		{
			rep_[a] = static_cast<unsigned char>(n >> 8);//取出高8位
			rep_[b] = static_cast<unsigned char>(n & 0xff);//取出低8位
			//相当于转换字节序,把小端格式转换为网络字节序
			//例如 数 0x1234 在小端模式(Little-endian)中表示为:
			//低地址---->高地址
			//34         12
			//网络序,大端模式(Big-endian)应该是:
			//12         34
			//该函数实现这个功能
		}

		unsigned short decode(int a, int b) const
		{
			return (rep_[a] << 8) + rep_[b];
			//这个就是encode的反函数,把两个字节倒过来返回
		}
		unsigned char rep_[28];
	};


	class DeviceMan{
	public:	
		struct DeviceNetWork
		{
			std::string _name;		//设备名字
			std::string _des;		//描述
			/*std::string _ip*/;		//ip地址
			std::vector<std::string> _ips;
			std::string _mac;		//mac地址
		};
		DeviceNetWork * operator [](size_t index);
		DeviceMan();
		~DeviceMan();
		size_t size() const { return _devicelst.size();}
	private:
		std::vector<DeviceNetWork *> _devicelst;
	};


	class ArpRunner:public yiqiding::Runnable
	{
	public:
		ArpRunner(DeviceMan::DeviceNetWork	*device);
		virtual ~ArpRunner();
		
		int request(const std::string& ip);
		void receive();

		void selected(int index);//for ip
		void start(){ _flag = true;  _thread.start();}
		void stop(){ _flag = false; _thread.join();_maps.clear();_multmaps.clear();}
		
	protected:
		virtual void normal_add(const std::string &ip , const std::string &mac){}
		virtual void exception_add(const std::string &ip , const std::string &mac){}
		virtual void normal_repeate(const std::string &ip , const std::string &mac){}

	private:
		DeviceMan::DeviceNetWork	*_deviceNetWork;
		pcap_t						*_fp;
		char						_errbuf[PCAP_ERRBUF_SIZE];
		ethernet_header				_eth;
		arp_header					_arp;
		std::map<std::string  , std::string> _maps;		//recode mac address
		std::multimap<std::string , std::string> _multmaps;
		yiqiding::Thread			_thread;
		bool						_flag;
		int							_index;
		void run();
};


}}
