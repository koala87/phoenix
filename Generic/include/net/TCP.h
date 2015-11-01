/**
 * TCP I/O
 * @author Shiwei Zhang
 * @date 2014.01.06
 */

#pragma once

#include "net/Socket.h"
#include "io/iostream.h"

namespace yiqiding { namespace net { namespace tcp {
	class Client : public Socket, public io::iostream {
	private:
		sockaddr_in _address;
		socklen_t _address_length;
	public:
		Client();
		Client(SOCKET socket, const sockaddr_in& addr, socklen_t addr_len);
		Client(Client&);
		~Client();
		void connect(const std::string& host, int port);

		Client & operator = (Client &c);

		// getter
		int getPort() const;
		std::string getAddress() const;

		// I/O
		virtual istream& read(char* s, size_t n);
		virtual ostream& write(const char* s, size_t n);
		

		int	readOnce(char *s , int n);
		int	writeOnce(const char *s , int n);
	};

	class Server : public Socket {
	private:
		int _port;
	public:
		Server();
		virtual ~Server();
		virtual void listen(int __port, int __backlog = 5);
		Client accept();
	};
}}}
