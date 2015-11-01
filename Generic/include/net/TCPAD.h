/**
 * High Performance TCP Asynchronous Duplex I/O
 * @author Shiwei Zhang
 * @date 2014.01.22
 */

#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
#include "MSTCPiP.h"
#include <Windows.h>
#include <memory.h>	// For memset
#include <map>
#include <set>
#include <queue>
#include "net/Socket.h"
#include "Thread.h"
#include "io/File.h"
#include "net/TelNet.h"

#ifdef USE_SECURE_SSL
#include "openssl/ssl.h"

#include "openssl/err.h"

#endif


namespace yiqiding { namespace net { namespace tcp { namespace async {

	const std::string SSL_PEM_PATH = "server.pem";
	

	// Enum Types
	enum {
		DEFAULT_READ_BUFFER_SIZE	=   1024,
		ACCEPT_ADDRESS_LENGTH       =   ((sizeof( struct sockaddr_in) + 16)),
	};

	enum CompletionKey {
		CK_NONE		=	0,
		CK_SHUTDOWN,
		CK_ACCEPT,
		CK_IO,
	};

	enum IOState {
		IO_NONE		=	0,
		IO_READ,
		IO_WRITE,
#ifdef USE_SOCK_POOL
		IO_DISCONNECTED,
#endif	
	};

	// Forward References
	class Connection;
	class ConnectionPool;

	// Listener Class
	class EventListener {
	public:
		virtual void onCompleteAccept(Connection* conn) {};
		virtual void onCompleteRead(Connection* conn, char* data, size_t size) {};
		virtual void onCompleteWrite(Connection* conn, char* data, size_t size) {};
		virtual void onConnectionLost(Connection* conn) {};
	};
	static EventListener DefaultEventListener;
	static EventListener* const DEFAULT_EVENT_LISTENER = &DefaultEventListener;

	// Socket Listener: Listen a certain port on the server
	class SocketListener : public Socket, public WSAOVERLAPPED {
	private:
#ifdef USE_SOCK_POOL	
		bool	_old;
#endif	
#ifdef USE_SECURE_SSL
		SSL_CTX	*_ctx;
#endif
		int		_port;
		HANDLE	_iocp;
		SOCKET	_acceptor;
		BYTE	_address[ACCEPT_ADDRESS_LENGTH * 2];
	public:
#ifdef USE_SECURE_SSL
		SocketListener(HANDLE iocp, int port, int back_queue_size , SSL_CTX	*_ctx = NULL);
#else
		SocketListener(HANDLE iocp, int port, int back_queue_size);
#endif
		~SocketListener();
		void	prepare();
		void	update();
		SOCKET	transfer();
		inline int		getPort() const		{ return _port; };
		inline SOCKET	getAcceptor()		{ return _acceptor; };
		inline BYTE*	getAddressBuffer()	{ return _address; };
#ifdef USE_SECURE_SSL
		inline SSL_CTX*	getCtx()	const   { return _ctx;}		
#endif

#ifdef  USE_SOCK_POOL	
		inline void 	setOld(bool old)	{ _old = old;};
		inline bool		getOld()	const	{ return _old;}	
#endif
	};

	// Classes
	class Channel : public WSAOVERLAPPED {
	protected:
		Connection* _parent_conn;
		IOState _state;
#ifdef USE_SOCK_POOL	
		SOCKET	_orgin;//for disconnected
#endif		
	public:

		Channel(Connection* parent) :_parent_conn(parent), _state(IO_NONE) {};
		virtual ~Channel() {}

		
		void prepare() { memset((WSAOVERLAPPED*)this, 0, sizeof(WSAOVERLAPPED)); };

		Connection* getConnection() { return _parent_conn; };
		IOState getState() const { return _state; };
		void setState(IOState io_state) { _state = io_state; };
#ifdef USE_SOCK_POOL
		//for disconnected
		void setOrgin(SOCKET orgin)	{_orgin = orgin;};
		SOCKET getOrgin() const {return _orgin;}
#endif
	};

	class Connection : public Socket {
	protected:
		// Structure Declaration
		struct Buffer {
			char* data;
			size_t size;
			size_t done;
		};

		// Core
		size_t				_id;
		ConnectionPool*		_pool;
		EventListener*		_event_listener;
		struct sockaddr_in	_address;
		int					_listen_port;

		// operator	using register 
		Mutex	_mutex;

#ifdef USE_SOCK_POOL
		// operator using shutdown
		Mutex	_mutexdown;
#endif

#ifdef USE_SECURE_SSL
		SSL			*_ssl;
		char		*_data;
		int			_len;
		int			_cur;
		BIO			*_wrd;
#endif

		// Ref control
		bool	_ref_closed;
		size_t	_ref_count;
		Mutex	_ref_mutex;

		// Input Channel
		Channel	_read_ch;
		char	_read_buf[DEFAULT_READ_BUFFER_SIZE];

		// Output Channel
		Channel				_write_ch;
		std::queue<Buffer>	_write_buf;
		Mutex				_write_mutex;
		Condition			_write_ready;
	public:
		Connection(SocketListener* listener, ConnectionPool* pool, EventListener* event_listener);
		virtual ~Connection();

		// Ref control
		void release();	// public use. Use after ConnectionPool::get()

		// (internal use)
		inline void referenceLock()		{ _ref_mutex.lock(); };
		inline void referenceUnlock()	{ _ref_mutex.unlock(); };
		inline void capture()			{ ++_ref_count;  };
		inline bool isCaptured()		{ return _ref_count > 0; };
		inline void markAsClosed()		{ _ref_closed = true; };
		inline bool isMarkedAsClosed()	{ return _ref_closed; };
		inline bool isExist()			{ return isCaptured() || _write_ch.getState() != IO_NONE || _read_ch.getState() != IO_NONE ;};

		// Operation
#ifdef USE_SECURE_SSL
		 static  int WINAPI bRead(BIO *b ,  char *data , int len)
		{
			Connection *conn = (Connection *)b->ptr;
			int left = conn->_len - conn->_cur;
			if(len >= left)
			{
				memcpy(data , conn->_data + conn->_cur , left);
				conn->_cur += left;
				return left;
			}
			else
			{
				memcpy(data , conn->_data + conn->_cur , len);
				conn->_cur += len;
				return len;
			}
		}

		 static  int  WINAPI bWrite(BIO *b , const char *data , int len)
		{
			Connection *conn = (Connection *)b->ptr;
			conn->transmit(data , len);
			return len;
		}			

		 void prepareRead(char *data , int len)
		 {
			 _data = data;
			 _len = len;
			 _cur = 0;
		 }

		 bool finishRead()
		 {
			return _cur >= _len;
		 }

		 int sslRead(char *data , int len)
		 {
			return SSL_read(_ssl , data , len);
		 }

         int sslWrite(char *data , int len)
		 {
			return SSL_write(_ssl , data , len);
		 }
#endif		
		
#ifdef USE_SOCK_POOL
		void shutdown();
#else
		inline void shutdown(){ ::shutdown(_socket, SD_BOTH); }
#endif

#ifdef USE_SECURE_SSL	
		bool SecureAccept();
		bool isSecured() { return _ssl != NULL;}
#endif

		void close();
		void tryClose();
		/// Send the data to remote. (Thread-Safe)
		/// This function will copy the data and push to the sending queue.
		void transmit(const char* data, size_t size);

		// Routine operation
		void startRead();
		void finishRead(size_t nTransferred);
		void startWrite();
		void finishWrite(size_t nTransferred);

		// Getter
		std::string	getAddress() const;
		int			getPort() const;
		std::string	getAddressPort() const;

		inline int			getListenPort()		{ return _listen_port; };
		inline Channel*		getInputChannel()	{ return &_read_ch; };	// Read Channel
		inline Channel*		getOutputChannel()	{ return &_write_ch; };	// Write Channel
		inline size_t		getID() const		{ return _id; };

		inline Mutex &		getMutex()			{ return _mutex;}

		// Setter
		inline void setID(size_t id)			{ _id = id; };
	};
	
	class ConnectionAllocator {
	public:
		virtual Connection* alloc(SocketListener* listener, ConnectionPool* pool, EventListener* event_listener) = 0;
	};

	/// Connection Pool (Thread safe)
	class ConnectionPool {
	private:
		size_t							_id;
		Mutex							_id_mutex;
		std::map<size_t, Connection*>	_pool;
		Mutex							_pool_mutex;
	public:
		ConnectionPool();
		virtual ~ConnectionPool();

		size_t		add(Connection* conn);
		void		close(size_t id);
		/// Get connection by connection id
		/// released() must be called after get()
		Connection*	get(size_t id);

		//telnet debug
		int showAllConnection(yiqiding::net::tel::ServerSend *srv);
		
	};

	/// High Performance TCP Asynchronous Duplex Server
	class Server : private io::FileHandle, private Runnable, public ConnectionPool {
	private:
		// Core
		size_t			_num_threads;
		Thread**		_threads;
		EventListener*	_event_listener;

		// Memory Allocator
		ConnectionAllocator*	_conn_alloc;

#ifdef USE_SECURE_SSL
		SSL_CTX	*		_ctx;
#endif

		// Listening (internal use)
		std::map<int, SocketListener*> _listeners;	// <Port, Listener>

		// Functions
		void run();
		void startAccept(SocketListener* listener);
		void finishAccept(SocketListener* listener);
		void onIoComplete(size_t nTransferred, Channel* channel);
	public:
		Server(size_t num_threads, EventListener* listener = DEFAULT_EVENT_LISTENER, ConnectionAllocator* conn_alloc = NULL);
		virtual ~Server();
#ifdef USE_SECURE_SSL
		virtual void listen(int port, int back_queue_size = 5 , bool bSecure =  false);
#else
		virtual void listen(int port, int back_queue_size = 5);
#endif
		virtual void stop();


	};

#ifdef USE_SECURE_SSL
	class SSLAccept:public yiqiding::Runnable
	{
	private:
		Connection *_conn;
	public:
		SSLAccept(Connection *conn);
		void run();
	};
#endif


}}}}
