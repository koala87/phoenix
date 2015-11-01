/*
* SocketPool
* @author Yuchun Zhang
* @date 2014.09.03
*/

#include "net/SocketPool.h"
#include "utility/Logger.h"
#include "net/TCP.h"
#include "utility/Utility.h"

using namespace yiqiding;




void SocketPool::push(SOCKET s)
{
	MutexGuard guard(_mutex);
	_sockQueue.push(s);
}


SOCKET SocketPool::pop()
{
	SOCKET  s = INVALID_SOCKET; 
	MutexGuard guard(_mutex);
	if(!_sockQueue.empty())
	{	
		s = _sockQueue.front();
		_sockQueue.pop();
	}

	return s;

}


ExternFun::ExternFun():_lpfnConnectEx(NULL),_lpfnDisconnectEx(NULL)
{
	GUID GuidDisconnectEx = WSAID_DISCONNECTEX;
	DWORD bytes;
	yiqiding::net::tcp::Client c;
	int rc;
	rc = WSAIoctl(c.native() , SIO_GET_EXTENSION_FUNCTION_POINTER ,
		&GuidDisconnectEx , sizeof(GuidDisconnectEx) , &_lpfnDisconnectEx ,
		sizeof(_lpfnDisconnectEx) , &bytes , NULL , NULL) ;
	if(rc == SOCKET_ERROR)
		yiqiding::utility::Logger::get("system")->log("Get DisconnectEx Address Error" , yiqiding::utility::Logger::WARNING);
	
	GUID GuidConnectEx = WSAID_CONNECTEX;
	rc = WSAIoctl(c.native() , SIO_GET_EXTENSION_FUNCTION_POINTER ,
		&GuidConnectEx , sizeof(GuidConnectEx) , &_lpfnConnectEx , 
		sizeof(_lpfnConnectEx) , &bytes , NULL , NULL); 
	if(rc == SOCKET_ERROR)
		yiqiding::utility::Logger::get("system")->log("Get ConnectEx Address Error" , yiqiding::utility::Logger::WARNING);
}
