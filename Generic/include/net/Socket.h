/**
 * Socket
 * @author Shiwei Zhang
 * @date 2014.01.13
 */

#pragma once

#include <WinSock2.h>
#include <Windows.h>

namespace yiqiding { namespace net {
	class Socket {
	public:
		class Driver {
		private:
			static Driver* _driver;
		public:
			Driver();
			~Driver();
			static void load();
			static void unload();
		};		
	protected:
		typedef int socklen_t;
		SOCKET _socket;
	public:
		Socket(SOCKET __socket = INVALID_SOCKET);
		Socket(Socket& __socket);	// transfer constructor
		//Socket(const Socket&);	// not implemented and never allowed
		virtual ~Socket();
		virtual void close();
		Socket& operator=(Socket& __socket);	// transfer

		inline SOCKET& native() { return _socket; };
		inline HANDLE asHandle() { return (HANDLE)_socket; };
		int getNRead();
	};
}}
