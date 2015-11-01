/**
 * MySQL C Connector C++ Wrapper
 * Based on MySQL C Connector 6.1.3 x64 build.
 * @warning No type convention in this wrapper. You are responsible for types.
 * @author Shiwei Zhang
 * @date 2014.02.08
 */

#pragma once

#include <string>
#include <memory>
#include <queue>
#pragma warning(push)
#pragma warning(disable:4251)
#include <cppconn/driver.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <cppconn/datatype.h>
#pragma warning(pop)
#include "Thread.h"

namespace yiqiding { namespace sql { namespace mysql {
	//////////////////////////////////////////////////////////////////////////
	// Type Definition
	//////////////////////////////////////////////////////////////////////////
	
	using ::sql::Driver;
	using ::sql::SQLException;
	using ::sql::DataType;

	//////////////////////////////////////////////////////////////////////////
	// Provide Limited Garbage Collection
	//////////////////////////////////////////////////////////////////////////

	//typedef std::auto_ptr<::sql::Connection>			Connection;
	class Connection : public std::auto_ptr<::sql::Connection> {
	public:
		Connection() {};
		Connection(::sql::Connection* s) : std::auto_ptr<::sql::Connection>(s) {};
		inline Connection& operator=(::sql::Connection* s) { reset(s); return *this; };
	};

	//typedef std::auto_ptr<::sql::Statement>			Statement;
	class Statement : public std::auto_ptr<::sql::Statement> {
	public:
		Statement() {};
		Statement(::sql::Statement* s) : std::auto_ptr<::sql::Statement>(s) {};
		inline Statement& operator=(::sql::Statement* s) { reset(s); return *this; };
	};

	//typedef std::auto_ptr<::sql::PreparedStatement>	PreparedStatement;
	class PreparedStatement : public std::auto_ptr<::sql::PreparedStatement> {
	public:
		PreparedStatement() {};
		PreparedStatement(::sql::PreparedStatement* s) : std::auto_ptr<::sql::PreparedStatement>(s) {};
		inline PreparedStatement& operator=(::sql::PreparedStatement* s) { reset(s); return *this; };
	};

	//typedef std::auto_ptr<::sql::ResultSet>			ResultSet;
	class ResultSet : public std::auto_ptr<::sql::ResultSet> {
	public:
		ResultSet() {};
		ResultSet(::sql::ResultSet* s) : std::auto_ptr<::sql::ResultSet>(s) {};
		inline ResultSet& operator=(::sql::ResultSet* s) { reset(s); return *this; };
	};

	//////////////////////////////////////////////////////////////////////////
	// Information Sector
	//////////////////////////////////////////////////////////////////////////

	class UserInfo {
	private:
		std::string _username;
		std::string _password;
	public:
		UserInfo() {};
		UserInfo(const std::string& username, const std::string& password) : _username(username), _password(password) {};
		virtual ~UserInfo() {};

		// Setter
		inline void setUsername(const std::string& username)	{ _username = username; };
		inline void setPassword(const std::string& password)	{ _password = password; };

		// Getter
		inline const std::string& getUsername() const	{ return _username; };
		inline const std::string& getPassword() const	{ return _password; };
	};

	class DatabaseInfo {
	private:
		std::string		_hostname;
		unsigned int	_port;
		std::string		_db_name;
	public:
		DatabaseInfo() : _port(0) {};
		DatabaseInfo(const std::string& hostname, const std::string& db_name, unsigned int port = 0) : _hostname(hostname), _port(port), _db_name(db_name) {};
		virtual ~DatabaseInfo() {};

		// Setter
		inline void setHostname(const std::string& hostname)	{ _hostname = hostname; };
		inline void setPort(unsigned int port)					{ _port = port; };
		inline void setDatabaseName(const std::string& db_name)	{ _db_name = db_name; };

		// Getter
		inline const std::string&	getHostname() const		{ return _hostname; };
		inline unsigned int			getPort() const			{ return _port; };
		inline const std::string&	getDatabaseName() const	{ return _db_name; };
	};

	class LoginInfo : public UserInfo, public DatabaseInfo {
	public:
		LoginInfo() {};
		LoginInfo(const UserInfo& user_info, const DatabaseInfo& db_info) : UserInfo(user_info), DatabaseInfo(db_info) {};
		LoginInfo(const UserInfo& user_info, const std::string& hostname, const std::string& db_name, unsigned int port = 0) : UserInfo(user_info), DatabaseInfo(hostname, db_name, port) {};
		LoginInfo(const DatabaseInfo& db_info, const std::string& username, const std::string& password) : UserInfo(username, password), DatabaseInfo(db_info) {};
		LoginInfo(const std::string& hostname, const std::string& username, const std::string& password, const std::string& db_name, unsigned int port = 0) : UserInfo(username, password), DatabaseInfo(hostname, db_name, port) {};
		virtual ~LoginInfo() {};

		Connection connect(const char* charset = NULL);
	};

	//////////////////////////////////////////////////////////////////////////
	// Operation Sector
	//////////////////////////////////////////////////////////////////////////
	
	/// Connection Pool (Thread-Safe).
	/// The singleton part is non-thread-safe.
	/// However, after one getInstance() call, it is thread safe.
	class ConnectionPool {
	private:
		Driver*							_driver;
		LoginInfo						_login_info;
		std::queue<::sql::Connection*>	_queue;
		Mutex							_queue_mutex;
		char*							_charset;
	public:
		ConnectionPool(const char* charset = NULL);
		ConnectionPool(const LoginInfo& login_info, const char* charset = NULL);
		~ConnectionPool();

		// Operations

		/// Get an alive connection from the pool.
		/// If nothing in the pool, it will create a new one.
		/// The caller takes the ownership of the Connection.
		Connection getConnection();
		/// Return a connection to the pool.
		/// The pool takes the ownership of the Connection.
		/// The caller should do nothing further on the Connection.
		void returnConnection(Connection conn);
		/// Clean the pool. Release all Connections
		void clear();

		// Setter
		void setLoginInfo(const LoginInfo& login_info)	{ _login_info = login_info; };
		void setCharset(const char* charset);
		
		// Getter
		const LoginInfo&	getLoginInfo() const	{ return _login_info; };

		//
		// Singleton Pattern (Non thread-safe)
		//

	private:
		static ConnectionPool* __instance;
	public:
		static ConnectionPool* getInstance();
		static void unload();
	};
}}}
