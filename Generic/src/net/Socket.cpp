/**
 * Socket
 * @author Shiwei Zhang
 * @date 2014.01.09
 */

#include "net/Socket.h"
#include "Exception.h"
#include "openssl/ssl.h"
using namespace yiqiding::net;
using yiqiding::Exception;

#pragma comment(lib,"ws2_32")   // Standard socket API.

Socket::Driver* Socket::Driver::_driver = NULL;

Socket::Driver::Driver() {
	// Initialize Winsock
	WSADATA wsaData;
	DWORD err_code;

	err_code = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (err_code != 0)
		throw Exception("WSAStartup", err_code, __FILE__, __LINE__);
#ifdef USE_SECURE_SSL
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
#endif
}

Socket::Driver::~Driver() {
	WSACleanup();
}

int Socket::getNRead()
{
	u_long nread = 0;
	if(ioctlsocket(0, FIONREAD, &nread) != SOCKET_ERROR)
		return nread;
	return -1;
}

void Socket::Driver::load() {
	if (_driver == NULL)
		_driver = new Driver;
}

void Socket::Driver::unload() {
	if (_driver != NULL) {
		delete _driver;
		_driver = NULL;
	}
}

Socket::Socket(SOCKET __socket) : _socket(__socket) {
	Socket::Driver::load();
}

Socket::Socket(Socket& __socket) {
	_socket = __socket._socket;
	__socket._socket = INVALID_SOCKET;
}

Socket::~Socket() {
	close();
}

void Socket::close() {
	closesocket(_socket);
	_socket = INVALID_SOCKET;
}

Socket& Socket::operator=(Socket& __socket) {
	if (this != &__socket) {
		close();
		_socket = __socket._socket;
		__socket._socket = INVALID_SOCKET;
	}
	return *this;
}


