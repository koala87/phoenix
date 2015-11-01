/*
* SocketPool
* @author Yuchun Zhang
* @date 2014.09.03
*/

#pragma once
#include <queue>
#include <WinSock2.h>
#include <Windows.h>
#include "mswsock.h"
#include "Thread.h"


template<class T>
class Singleton
{
private:
	static T	*_instance;
	static yiqiding::Mutex	_mutex;
	Singleton(){};
public:
	static T * getInstance();
	static void unLoad();
};

template<class T> 
T* Singleton<T>::getInstance()
{
	if(_instance == NULL)	
	{
		yiqiding::MutexGuard guard(_mutex);
		_instance = new T();
	}
	return _instance;
}

template<class T>
void Singleton<T>::unLoad()
{
	yiqiding::MutexGuard guard(_mutex);
	if (_instance != NULL)
	{
		delete _instance;
		_instance = NULL;
	}
}

template<class T>
T * Singleton<T>::_instance = NULL;

template<class T>
yiqiding::Mutex Singleton<T>::_mutex;

class SocketPool
{
private:
	yiqiding::Mutex			_mutex; 
	std::queue<SOCKET>		_sockQueue;
	SocketPool(){}
public:
	void push(SOCKET s);
	SOCKET pop();
	friend class Singleton<SocketPool>;
};



class ExternFun
{
private:
	ExternFun();
public:
	LPFN_DISCONNECTEX	_lpfnDisconnectEx;
	LPFN_CONNECTEX		_lpfnConnectEx;
	
	friend class Singleton<ExternFun>;

};

