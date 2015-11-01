/**
 * High Performance TCP Asynchronous Duplex I/O Implementation
 * @author Shiwei Zhang
 * @date 2014.01.22
 */

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include <sstream>
#include "net/TCPAD.h"
#include "Exception.h"
#include "utility/Utility.h"
#include "utility/Logger.h"
#include "net/SocketPool.h"

using namespace yiqiding::net::tcp::async;
using namespace yiqiding::utility;
using yiqiding::Exception;

#pragma comment(lib,"ws2_32")   // Standard socket API.
#pragma comment(lib,"mswsock")  // AcceptEx, TransmitFile, etc,.

#pragma warning(disable:4355)	// 'this' : used in base member initializer list

//////////////////////////////////////////////////////////////////////////
// SocketListener
//////////////////////////////////////////////////////////////////////////
#ifdef USE_SECURE_SSL
SocketListener::SocketListener(HANDLE iocp, int port, int back_queue_size , SSL_CTX *ctx) : _port(port), _iocp(iocp), _acceptor(INVALID_SOCKET),_ctx(ctx)
#else
SocketListener::SocketListener(HANDLE iocp, int port, int back_queue_size) : _port(port), _iocp(iocp), _acceptor(INVALID_SOCKET)
#endif
#ifdef USE_SOCK_POOL
	,_old(false)
#endif
{
	// Initialize socket listeners
	_socket = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == _socket)
		throw Exception("Server: WSASocket", WSAGetLastError(), __FILE__, __LINE__);

	struct sockaddr_in servaddr = {0};
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.S_un.S_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);

	// Bind the listener to the local interface and set to listening state.
	if (bind(_socket, (struct sockaddr *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
		throw Exception("bind", WSAGetLastError(), __FILE__, __LINE__);

	if(listen(_socket, back_queue_size) == SOCKET_ERROR)
		throw Exception("listen", WSAGetLastError(), __FILE__, __LINE__);

	if (CreateIoCompletionPort((HANDLE)_socket, iocp, CK_ACCEPT, 0) == NULL)
		throw Exception("Server: CreateIoCompletionPort", GetLastError(), __FILE__, __LINE__);
}

SocketListener::~SocketListener() {
	shutdown(_socket, SD_BOTH);
	closesocket(_acceptor);
}

void SocketListener::prepare() {
#ifdef USE_SOCK_POOL
	_acceptor = Singleton<SocketPool>::getInstance()->pop();
	if(_acceptor != INVALID_SOCKET)
		setOld(true);
	else
		_acceptor = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
#else
	_acceptor = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
#endif

	if (INVALID_SOCKET == _acceptor)
		throw Exception("Server: WSASocket", WSAGetLastError(), __FILE__, __LINE__);
	memset((WSAOVERLAPPED*)this, 0, sizeof(WSAOVERLAPPED));
}

void SocketListener::update() {
	setsockopt(_acceptor, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&_socket, sizeof(SOCKET));
#ifdef USE_SOCK_POOL
	if (!getOld() && CreateIoCompletionPort((HANDLE)_acceptor, _iocp, CK_IO, 0) == NULL)
		throw Exception("Server: CreateIoCompletionPort", GetLastError(), __FILE__, __LINE__);
	setOld(false);
#else
	if ( CreateIoCompletionPort((HANDLE)_acceptor, _iocp, CK_IO, 0) == NULL)
		throw Exception("Server: CreateIoCompletionPort", GetLastError(), __FILE__, __LINE__);	
#endif
	

	//keepalive 
	
	tcp_keepalive tKep = {0} , sRet = {0};
	DWORD dwBytes = 0;
	tKep.onoff = 1;
	tKep.keepalivetime = 30000;
	tKep.keepaliveinterval = 3000;
	if (WSAIoctl(_acceptor , SIO_KEEPALIVE_VALS , &tKep ,sizeof(tKep) ,
		&sRet , sizeof(sRet) ,&dwBytes ,NULL , NULL) != 0)
	{
		throw Exception("WSAIoctl SIO_KEEPALIVE_VALS" , GetLastError() , __FILE__ , __LINE__);
	}
	
}

SOCKET SocketListener::transfer() {
	SOCKET socket = _acceptor;
	_acceptor = INVALID_SOCKET;
	return socket;
}

//////////////////////////////////////////////////////////////////////////
// Connection
//////////////////////////////////////////////////////////////////////////
Connection::Connection(SocketListener* listener, ConnectionPool* pool, EventListener* event_listener) :
Socket(listener->transfer()), _id(0), _pool(pool), _event_listener(event_listener), _listen_port(listener->getPort()),
	_ref_closed(false), _ref_count(0),
	_read_ch(this), _write_ch(this)
#ifdef USE_SECURE_SSL
	, _ssl(NULL)
#endif
{
	int address_length = sizeof(_address);
	getpeername(_socket, (sockaddr*)&_address, &address_length);
#ifdef USE_SECURE_SSL
	if(listener->getCtx())
		_ssl = SSL_new(listener->getCtx());
#endif
}

Connection::~Connection() {
	shutdown();

	// Generate Event
	_event_listener->onConnectionLost(this);

	// Clean up
	while (!_write_buf.empty()) {
		delete [] _write_buf.front().data;
		_write_buf.pop();
	}
#ifdef USE_SECURE_SSL
	if(_ssl != NULL)
	{
		SSL_free(_ssl);
		_ssl = NULL;
	}
#endif

}

void Connection::release() {
	_ref_mutex.lock();
	_ref_count--;
	if (!isExist() && _ref_closed) {
		_ref_mutex.unlock();
		delete this;
	} else
		_ref_mutex.unlock();
}

void Connection::close() {
	// Clear to delete Connection
	_pool->close(_id);

	// At this point, the instance has been deleted. No more operations on the object.
	// Unless someone has captured this connection. In such a case, this connection
	// will be deleted after invoking release();
}

void Connection::tryClose() {

	Connection::close();

}

void Connection::transmit(const char* data, size_t size) {
	char* buffer_data = new char[size];
	memcpy(buffer_data, data, size);

	Buffer buffer = { buffer_data, size, 0 };

	bool do_write = false;
	{
		MutexGuard guard(_write_mutex);
		_write_buf.push(buffer);
		if (_write_buf.size() == 1)	// Queue was empty
			do_write = true;
	}
	if (do_write)	// Since queue was empty, we have to make a write trigger
		startWrite();
}

void Connection::startRead() {
	int ret;
	DWORD flags = 0;
	WSABUF wsabuf = {DEFAULT_READ_BUFFER_SIZE, _read_buf};

	_read_ch.prepare();
	_read_ch.setState(IO_READ);
	if ((ret = WSARecv(_socket, &wsabuf, 1, NULL, &flags, &_read_ch, NULL))	== SOCKET_ERROR) {
		int err_code = WSAGetLastError();
		switch (err_code) {
		case WSA_IO_PENDING:
			break;
		case WSAECONNABORTED:
		case WSAESHUTDOWN:
			_read_ch.setState(IO_NONE);
			tryClose();
			break;
		default:
			_read_ch.setState(IO_NONE);
			tryClose();
			throw Exception(getAddress() + ": WSARecv", err_code, __FILE__, __LINE__);
		}
	}
}

void Connection::finishRead(size_t nTransferred) {
	if (nTransferred > 0) {
		try {
#ifdef USE_SECURE_SSL 
			char read_buf[DEFAULT_READ_BUFFER_SIZE];
			int len = 0;
			if(isSecured())
			{
				prepareRead(_read_buf , nTransferred);
				do{				//only once , test
					len = sslRead(read_buf , DEFAULT_READ_BUFFER_SIZE);
					if(len > 0)
					{	
						_event_listener->onCompleteRead(this , read_buf , len);
					}
					else
					{	
						yiqiding::utility::Logger::get("system")->log("sslRead "  + yiqiding::utility::toString(len)); 
						_read_ch.setState(IO_NONE);
						tryClose();
						return;
					}
				}while(!finishRead());

			}
			else
			{
				_event_listener->onCompleteRead(this, _read_buf, nTransferred);
			}
#else	
			_event_listener->onCompleteRead(this, _read_buf, nTransferred);
#endif
		} catch (const std::exception& e) {
			Logger::get("system")->log(e.what(), Logger::WARNING);
		}
		// Keep the state of IO_READ
		// Next!
		startRead();
	} else {	// connection closed by client
		
		_read_ch.setState(IO_NONE);
		tryClose();
	}
}

void Connection::startWrite() {
	int ret;
	DWORD flags = 0;
	Buffer buffer;
	{
		MutexGuard guard(_write_mutex);
		buffer = _write_buf.front();
	}
	WSABUF wsabuf = {(ULONG)(buffer.size - buffer.done), buffer.data + buffer.done};

	referenceLock();
	capture();
	referenceUnlock();

	_write_ch.prepare();
	_write_ch.setState(IO_WRITE);
	if ((ret = WSASend(_socket, &wsabuf, 1, NULL, 0, &_write_ch, NULL)) == SOCKET_ERROR) {
		int err_code = WSAGetLastError();
		if (err_code != WSA_IO_PENDING)
		{	
			_write_ch.setState(IO_NONE);
			release();
			throw Exception(getAddress() + ": WSASend", err_code, __FILE__, __LINE__);
		}
	}


}

void Connection::finishWrite(size_t nTransferred) {
	if (nTransferred > 0) {
		Buffer buffer;
		{
			MutexGuard guard(_write_mutex);
			buffer = _write_buf.front();
		}
		buffer.done += nTransferred;
		if (buffer.size <= buffer.done) {
			bool do_write = false;
			// Packet send complete
			{
				MutexGuard guard(_write_mutex);
				_write_buf.pop();
				if (_write_buf.empty())
				{	
					_write_ch.setState(IO_NONE);
				}
				else
					do_write = true;
			}
			

			// Further Process
			_event_listener->onCompleteWrite(this, buffer.data, buffer.size);

			// Clean up
			delete [] buffer.data;


			if (do_write)	// SO... NEXT!
			{	
				release();
				startWrite();
			}
			else
			{
				release();
			}
			
			
		} else {
			// Packet send incomplete. continue!
			{
				MutexGuard guard(_write_mutex);
				_write_buf.front() = buffer;
			}
			startWrite();
		}
	} else {	// connection reset by client
		
		_write_ch.setState(IO_NONE);
		release();
	}	
}


#ifdef USE_SOCK_POOL

void Connection::shutdown()
{
	MutexGuard guard(_mutexdown);
	if(_socket != NULL)
	{
		Channel *channel = new Channel(NULL);
		channel->prepare();
		channel->setOrgin(_socket);
		channel->setState(IO_DISCONNECTED);
		if(Singleton<ExternFun>::getInstance()->_lpfnDisconnectEx(_socket ,channel , TF_REUSE_SOCKET , 0) == FALSE )
		{
	
			int err = GetLastError(); 
			switch(err)
			{
			case ERROR_IO_PENDING:
				break;
			default:
				::shutdown(_socket , SD_BOTH);
				yiqiding::utility::Logger::get("system")->log("DisconnectEx err: " + yiqiding::utility::toString(err) , yiqiding::utility::Logger::WARNING);

			}	
		}
		_socket = NULL;
	}
}

#endif

std::string Connection::getAddress() const {
	std::ostringstream sout;

	sout << int(_address.sin_addr.s_addr & 0xFF) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF00) >> 8) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF0000) >> 16) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF000000) >> 24);

	return sout.str();
}

int Connection::getPort() const {
	return ntohs(_address.sin_port);
}
#ifdef USE_SECURE_SSL
bool Connection::SecureAccept()
{ 
	SSL_set_fd(_ssl , native());
 

	
	DWORD   dwTimeout=1000; //设置超时，因为SSL_accept是阻塞接口，防止有人，不进行shake，阻塞线程。SSL_accept之后，取消之。 
	setsockopt(native(),SOL_SOCKET,SO_RCVTIMEO,(char*)&dwTimeout,sizeof dwTimeout); 
	setsockopt(native(),SOL_SOCKET,SO_SNDTIMEO,(char*)&dwTimeout,sizeof dwTimeout); 
	
	int err = SSL_accept(_ssl);
	
	dwTimeout = 0;		
	setsockopt(native(),SOL_SOCKET,SO_RCVTIMEO,(char*)&dwTimeout,sizeof dwTimeout); 
	setsockopt(native(),SOL_SOCKET,SO_SNDTIMEO,(char*)&dwTimeout,sizeof dwTimeout); 

	if(err == -1)
	{
		yiqiding::utility::Logger::get("system")->log("SSL_accept Error");
		return false;
	}
	
	X509 *client_cert = SSL_get_peer_certificate(_ssl);
	if(client_cert == NULL)
	{
		//ERR_print_errors_fp(stderr);
		Logger::get("system")->log("SSL_get_peer_certificate NULL" , Logger::WARNING);
		return false;
	}
	
	if(client_cert != NULL)
	{

		X509_NAME *name = X509_get_subject_name(client_cert);
		{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_commonName , buf , 32);
			//printf("NID_commonName:%s\n" , buf);
		}

		{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_countryName , buf , 32);
			//printf("NID_countryName:%s\n" , buf);
		}

	/*	{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_localityName , buf , 32);
			printf("NID_localityName:%s\n" , buf);
		}
	*/
		{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_stateOrProvinceName , buf , 32);
			//printf("NID_stateOrProvinceName:%s\n" , buf);
		}

		{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_organizationName , buf , 32);
			//printf("NID_organizationName:%s\n" , buf);
		}

		
		{
			char buf[32];
			X509_NAME_get_text_by_NID(name , NID_organizationalUnitName , buf , 32);
			//printf("NID_organizationalUnitName:%s\n" , buf);
		}
		
		X509_free(client_cert);
	}
	else{ return false;}

	if (SSL_get_verify_result(_ssl) != X509_V_OK)
	{
		Logger::get("system")->log("SSL_get_verify_result != X509_V_OK" , Logger::WARNING);
		return false;
	}
	
	_wrd = BIO_new(BIO_f_null());
	_wrd->ptr = this;
	_wrd->method->bread = Connection::bRead;
	_wrd->method->bwrite = Connection::bWrite;

	SSL_set_bio(_ssl , _wrd , _wrd);

	return true;
}
#endif

std::string	Connection::getAddressPort() const {
	std::ostringstream sout;

	sout << int(_address.sin_addr.s_addr & 0xFF) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF00) >> 8) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF0000) >> 16) << '.'
		<< int((_address.sin_addr.s_addr & 0xFF000000) >> 24) << ':'
		<< ntohs(_address.sin_port);

	return sout.str();
}

//////////////////////////////////////////////////////////////////////////
// ConnectionPool
//////////////////////////////////////////////////////////////////////////

ConnectionPool::ConnectionPool() : _id(0) {}

ConnectionPool::~ConnectionPool() {
	for (std::map<size_t, Connection*>::iterator i = _pool.begin(); i != _pool.end(); ++i)
		delete i->second;
}

size_t ConnectionPool::add(Connection* conn) {
	while (true) {
		// Obtain id
		size_t id;
		{
			MutexGuard lock(_id_mutex);
			id = _id++;
		}
		// Register in the pool
		{
			MutexGuard lock(_pool_mutex);
			if (_pool.count(id))
				continue;	// ID exists! Retry!
			_pool[id] = conn;
			conn->setID(id);
		}
		return id;
	}
}

void ConnectionPool::close(size_t id) {


	Connection* conn = NULL;
	{
		MutexGuard lock(_pool_mutex);
		std::map<size_t, Connection*>::iterator conn_itr = _pool.find(id);
		if (conn_itr == _pool.end())	// ID not found
			return;
		else {	// Remove from the pool
			conn = conn_itr->second;
			_pool.erase(conn_itr);
		}
	}

	// Close Connection
	conn->referenceLock();
	if (conn->isExist()) {	
		// Connection is still using
		conn->markAsClosed();
		conn->referenceUnlock();
	} else {
		conn->referenceUnlock();
		delete conn;
	}
}

Connection* ConnectionPool::get(size_t id) {
	MutexGuard lock(_pool_mutex);
	std::map<size_t, Connection*>::iterator conn_itr = _pool.find(id);

	if (conn_itr == _pool.end())
		return NULL;	// ID not found
	else {
		Connection* conn = conn_itr->second;
		conn->referenceLock();
		if (conn->isMarkedAsClosed()) {
			conn->referenceUnlock();
			return NULL;
		} else {
			conn->capture();
			conn->referenceUnlock();
			return conn;
		}
	}
}

int ConnectionPool::showAllConnection(yiqiding::net::tel::ServerSend *srv)
{
	std::ostringstream out;
	{
		out << "Connection:\r\n";
		MutexGuard lock(_pool_mutex);
		std::map<size_t , Connection *>::iterator it = _pool.begin();
		while(it != _pool.end())
		{
			out <<"CON_ID:"<<it->second->getID()<<"\t"<<it->second->getAddressPort()<<"\t"<<it->second->getListenPort()<<"\r\n";
			it++;
		}
		srv->teleSend(out.str());
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////
// Server
//////////////////////////////////////////////////////////////////////////

void Server::run() {
	BOOL retval = 0;
	DWORD nTransferred = 0;
	ULONG_PTR key = CK_NONE;
	LPOVERLAPPED ovl = NULL;
	try {
		while (true) {
			if (GetQueuedCompletionStatus(_handle, &nTransferred, &key, &ovl, INFINITE) == FALSE) {
				// I/O Failure
				switch (key) {
				case CK_IO:	// Fail R/W
					{
						Channel* channel = (Channel*)ovl;
						
						// Check Connection
						Connection* conn = channel->getConnection();
						if(channel->getState() == IO_READ)
						{
							channel->setState(IO_NONE);
							conn->tryClose();
						}
						else if (channel->getState() == IO_WRITE)
						{
							channel->setState(IO_NONE);
							conn->release();
						}
#ifdef USE_SOCK_POOL
						else if(channel->getState() == IO_DISCONNECTED)
						{
							//Singleton<SocketPool>::getInstance()->push(channel->getOrgin());
							delete channel;
						}
#endif
					}
					break;
				case CK_ACCEPT:	// Fail Accept
					try {
						startAccept((SocketListener*)ovl);	// Re-accept
					} catch (const Exception& e) {
						Logger::get("system")->log(e.what(), Logger::WARNING);
					}
					break;
				default:
					// Something wrong. Should not happen!!!
					throw Exception("Server: Thread: GetQueuedCompletionStatus", GetLastError(), __FILE__, __LINE__);
				}
			} else {	// Normal I/O
				
				switch (key) {
				case CK_IO:
					try {
						onIoComplete(nTransferred, (Channel *)ovl);
					} catch (const Exception& e) {
						Logger::get("system")->log( e.what(), Logger::WARNING);
					}
					break;
				case CK_ACCEPT:
					try {
						finishAccept((SocketListener*)ovl);
					} catch (const Exception& e) {
						Logger::get("system")->log(e.what(), Logger::WARNING);
					}
					break;
				case CK_SHUTDOWN:	// Thread done
					return;
				default:
					{	// Something wrong. Should not happen!!!
						Exception e("Server: Thread: GetQueuedCompletionStatus", "Unknown key", __FILE__, __LINE__);
						Logger::get("system")->log(e.what(), Logger::WARNING);
					}
				}

		
			}
		}
	} catch (const std::exception& e) {
		try {
			Logger::get("system")->log(e.what(), Logger::FATAL);
		} catch (const std::exception& e) {
#ifdef _DEBUG
			std::cerr << e.what() << std::endl;
#else
			UNREFERENCED_PARAMETER(e);
#endif // _DEBUG
		}
	}
}

void Server::startAccept(SocketListener* listener) {
	listener->prepare();

	while (true) {
		if (AcceptEx(listener->native(), listener->getAcceptor(), listener->getAddressBuffer(), 0, ACCEPT_ADDRESS_LENGTH, ACCEPT_ADDRESS_LENGTH, NULL, listener) == FALSE) {	
			DWORD err_code = WSAGetLastError();
			switch (err_code) {
			case ERROR_IO_PENDING:
				return;
			case WSAECONNRESET:
				continue;
			default:
				throw Exception("Server: AcceptEx", err_code, __FILE__, __LINE__);
			}
		} else
			return;
	}
}

void Server::finishAccept(SocketListener* listener) {
	listener->update();
	
	Connection* conn = (_conn_alloc) ? _conn_alloc->alloc(listener, this, _event_listener) : new Connection(listener, this, _event_listener);
	ConnectionPool::add(conn);
#ifdef USE_SECURE_SSL
	bool secured = conn->isSecured();
#endif
	// Next!
	startAccept(listener);

	// Further process
	_event_listener->onCompleteAccept(conn);	//this maybe ~conn in secureAccept , if secured is true.
#ifdef USE_SECURE_SSL
	if(!secured)
#endif
	// Let the new connection in the state of reading
	conn->startRead();

}

void Server::onIoComplete(size_t nTransferred, Channel* channel) {
	Connection* conn = channel->getConnection();


	switch (channel->getState()) {
	case IO_READ:
		conn->finishRead(nTransferred);
		break;
	case IO_WRITE:
		conn->finishWrite(nTransferred);
		break;
#ifdef USE_SOCK_POOL
	case IO_DISCONNECTED:
		{
			SOCKET s = channel->getOrgin();
			delete channel;
			Singleton<SocketPool>::getInstance()->push(s);
			
		}
		break;
#endif
	default:
		// Error!!! Disconnect if possible
		Logger::get("system")->log(conn->getAddress() + ": unknown channel detected", Logger::NORMAL);
		//conn->tryClose();
		break;
	}


}

Server::Server(size_t num_threads, EventListener* listener, ConnectionAllocator* conn_alloc) :
#ifdef USE_SECURE_SSL
	_ctx(NULL),
#endif
_event_listener(listener), _num_threads(num_threads), _threads(NULL), _conn_alloc(conn_alloc)
{
	Socket::Driver::load();
	


	_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, (DWORD)_num_threads);
	if (NULL == _handle)
		throw Exception("Server: CreateIoCompletionPort", GetLastError(), __FILE__, __LINE__);
}

Server::~Server() {
	if (_threads != NULL) {
		for (size_t i = 0; i < _num_threads; i++)
			delete _threads[i];
		delete [] _threads;
	}
#ifdef USE_SECURE_SSL
	if(_ctx != NULL)
	{
		SSL_CTX_free(_ctx);
		_ctx = NULL;
	}
#endif

}
#ifdef USE_SECURE_SSL
void Server::listen(int port, int back_queue_size /* = 5  */, bool bSecure /* = false */){
#else
void Server::listen(int port, int back_queue_size) {
#endif
	// Create worker threads (if not)
	if (_threads == NULL) {
		_threads = new Thread*[_num_threads]();
		for (size_t i = 0; i < _num_threads; i++) {
			_threads[i] = new Thread(this);
			_threads[i]->start();
		}
	}
#ifdef USE_SECURE_SSL
	if(bSecure && _ctx == NULL)
	{
		_ctx = SSL_CTX_new(SSLv23_server_method());
		if(NULL == _ctx)
			throw Exception("Server: SSL_CTX_new Error" , __FILE__ , __LINE__);
	
		if (SSL_CTX_load_verify_locations( _ctx , "ca.crt", NULL) <= 0)
			throw Exception("SSL_CTX_load_verify_locations Error" , __FILE__ , __LINE__);
		SSL_CTX_set_verify(_ctx , SSL_VERIFY_PEER , NULL);
		SSL_CTX_set_verify_depth(_ctx , 1);


		if( SSL_CTX_use_certificate_file(_ctx ,SSL_PEM_PATH.c_str() , SSL_FILETYPE_PEM) <= 0)
			throw Exception("SSL_CTX_use_certificate_file Error" , __FILE__ , __LINE__);
		SSL_CTX_set_default_passwd_cb_userdata(_ctx , "112358");		
		if(SSL_CTX_use_PrivateKey_file(_ctx , SSL_PEM_PATH.c_str() , SSL_FILETYPE_PEM) <= 0)
			throw Exception("SSL_CTX_use_PrivateKey_file Error" , __FILE__ , __LINE__);
		if( !SSL_CTX_check_private_key(_ctx))
			throw Exception("SSL_CTX_check_private_key Error" , __FILE__ , __LINE__);
	}
#endif

	// Check listeners
	if (_listeners.count(port))
		throw Exception("Server: listen", "port listened", __FILE__, __LINE__);

	// Initialize socket listeners
#ifdef USE_SECURE_SSL
	SocketListener* listener;
	if(bSecure && _ctx != NULL)
		listener = new SocketListener(_handle, port, back_queue_size , _ctx);
	else
		listener = new SocketListener(_handle, port, back_queue_size);
#else
	SocketListener* listener = new SocketListener(_handle, port, back_queue_size);
#endif
	_listeners[port] = listener;

	// Initiate Running
	startAccept(listener);
}

void Server::stop() {
	for (size_t i = 0; i < _num_threads; i++)
		PostQueuedCompletionStatus(_handle, 0, CK_SHUTDOWN, 0);
	
	// Wait all threads
	for (size_t i = 0; i < _num_threads; i++)
		_threads[i]->join();

	// Clean up
	for (std::map<int, SocketListener*>::iterator i = _listeners.begin(); i != _listeners.end(); ++i)
		delete i->second;
	_listeners.clear();

	if (_threads != NULL) {
		for (size_t i = 0; i < _num_threads; i++)
			delete _threads[i];
		delete [] _threads;
		_threads = NULL;
	}
}

#ifdef USE_SECURE_SSL


void SSLAccept::run()
{
	if(_conn->SecureAccept() )
	{
		yiqiding::utility::Logger::get("system")->log("Connection " + toString(_conn->getID()) + " SSL Accept " + _conn->getAddressPort());
		_conn->startRead();
	}
	else
	{
		yiqiding::utility::Logger::get("system")->log("Connection " + toString(_conn->getID()) + " SSL Accept Error");
		_conn->close();
	}

}

SSLAccept::SSLAccept(Connection *conn):_conn(conn){}

#endif