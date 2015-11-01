/**
 * TCP I/O Implementation
 * @author Shiwei Zhang , Yuchun Zhang
 * @date 2014.03.14
 */
#include <ws2tcpip.h>
#include <sstream>
#include "net/TCP.h"
#include "Exception.h"
using namespace yiqiding::net::tcp;
using namespace yiqiding::io;
using yiqiding::Exception;

// Client
Client::Client() {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw Exception("socket", GetLastError(), __FILE__, __LINE__);
}

Client::Client(SOCKET socket, const sockaddr_in& addr, socklen_t addr_len) :
Socket(socket), _address(addr), _address_length(addr_len) {}

Client::Client(Client& __c) : Socket(__c),_address(__c._address), _address_length(__c._address_length)
{

}

Client & Client::operator=(Client &c)
{
	if (&c != this)
	{	
		close();
		_address = c._address;
		_address_length = c._address_length;
		_socket = c._socket;
		c._socket = INVALID_SOCKET;
		is_eof = c.is_eof;
	}
	return *this;
}

Client::~Client() {}

void Client::connect(const std::string& host, int port) {
	// Get the address of the host
/*	hostent* hostPtr = gethostbyname(host.c_str());
	if (hostPtr == NULL) {
		DWORD err_code = WSAGetLastError();
		switch (err_code) {
		case WSAHOST_NOT_FOUND:
		case WSANO_DATA:
			throw Exception("connect", "unknown host: " + host, __FILE__, __LINE__);
		default:
			throw Exception("connect", err_code, __FILE__, __LINE__);
		}
	}
	if(hostPtr->h_addrtype !=  AF_INET)
		throw Exception("connect", "unknown address type",__FILE__, __LINE__);

	_address_length = sizeof(_address);
	memset(&_address, 0, _address_length);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = ((in_addr*)hostPtr->h_addr_list[0])->s_addr;
	_address.sin_port = htons(port);

	// Connect to the server
	if (::connect(_socket, (sockaddr*)&_address, _address_length) == SOCKET_ERROR)
		throw Exception("connect", "unable to connect to server: " + host, __FILE__, __LINE__);*/


	// use getaddrinfo thread safe
	struct addrinfo hint , *result = NULL;
	memset(&hint,0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_protocol = IPPROTO_TCP;
	int dwRetval =  getaddrinfo(host.c_str() , NULL , &hint ,&result);
	if(dwRetval != 0 || result == NULL)
	{
		DWORD err_code = WSAGetLastError();
		if(result != NULL)
			freeaddrinfo(result);
		throw Exception("connect", err_code, __FILE__, __LINE__);
	}

	_address_length = sizeof(_address);
	memset(&_address, 0, _address_length);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = ((SOCKADDR_IN*)result->ai_addr)->sin_addr.s_addr;
	_address.sin_port = htons(port);

	// Connect to the server
	if (::connect(_socket, (sockaddr*)&_address, _address_length) == SOCKET_ERROR)
	{	
		freeaddrinfo(result);
		throw Exception("connect", "unable to connect to server: " + host, __FILE__, __LINE__);
	}

	freeaddrinfo(result);
	
}

int Client::getPort() const {
	return ntohs(_address.sin_port);
}

std::string Client::getAddress() const {
	std::ostringstream sout;

	sout << int(_address.sin_addr.s_addr & 0xFF) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF00) >> 8) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF0000) >> 16) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF000000) >> 24);

	return sout.str();
}

int Client::readOnce(char *s , int n)
{
	return recv(_socket , s , n , 0);
}


istream& Client::read(char* s, size_t n) {
	int nleft, nread;

	nleft = (int)n;
	while (nleft > 0) {
		nread = ::recv(_socket, s, nleft, 0);
		if (nread == SOCKET_ERROR)
			throw Exception("read", WSAGetLastError(), __FILE__, __LINE__);
		// nread == 0 indicates EOF: no more data available for reading.
		else if (nread == 0) {
			is_eof = true;
			break;
		}
		nleft -= nread;
		s += nread;
	}

	return *this;
}

int Client::writeOnce(const char *s , int n)
{
	return send(_socket , s , n , 0);
}

ostream& Client::write(const char* s, size_t n) {
	int nleft, nwritten;

	nleft = (int)n;
	while (nleft > 0) {
		nwritten = ::send(_socket, s, nleft, 0);
		if (nwritten == SOCKET_ERROR)
			throw Exception("write", WSAGetLastError(), __FILE__, __LINE__);
		nleft -= nwritten;
		s += nwritten;
	}

	return *this;
}

// Server
Server::Server() : _port(0) {
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket == INVALID_SOCKET)
		throw Exception("socket", GetLastError(), __FILE__, __LINE__);
}

Server::~Server() {}

void Server::listen(int __port, int __backlog) {
	_port = __port;

	struct sockaddr_in servaddr;
	char on = 1;

	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == SOCKET_ERROR)
		throw Exception("setsockopt", WSAGetLastError(), __FILE__, __LINE__);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(_port);

	if (bind(_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
		throw Exception("bind", WSAGetLastError(), __FILE__, __LINE__);

	if(::listen(_socket, __backlog) == SOCKET_ERROR)
		throw Exception("listen", WSAGetLastError(), __FILE__, __LINE__);
}

Client Server::accept() {
	SOCKET client_socket;
	sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	client_socket = ::accept(_socket, (sockaddr*)&client_addr, &client_addr_len);
	if (client_socket == SOCKET_ERROR)
		throw Exception("accept", WSAGetLastError(), __FILE__, __LINE__);

	return Client(client_socket, client_addr, client_addr_len);
}
