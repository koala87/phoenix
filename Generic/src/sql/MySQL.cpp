/**
 * MySQL C Connector C++ Wrapper Implementation
 * Based on MySQL C Connector 6.1.3 x64 build
 * @author Shiwei Zhang
 * @date 2014.02.08
 */

#pragma warning(push)
#pragma warning(disable:4251)
#include <mysql_driver.h>
#pragma warning(pop)
#include "sql/MySQL.h"
#include "utility/Utility.h"
using namespace yiqiding::sql::mysql;
using namespace yiqiding::utility;
using yiqiding::MutexGuard;

#pragma comment(lib, "mysqlcppconn")

//////////////////////////////////////////////////////////////////////////
// LoginInfo
//////////////////////////////////////////////////////////////////////////

Connection LoginInfo::connect(const char* charset) {
		Connection conn = ::sql::mysql::get_driver_instance()->connect("tcp://" + getHostname() + ":" + utility::toString(getPort()), getUsername(), getPassword());
		conn->setSchema(getDatabaseName());
		if (charset) {
			Statement stmt = conn->createStatement();
			stmt->execute(std::string("SET NAMES ") + charset);
		}
		return conn;
};

//////////////////////////////////////////////////////////////////////////
// ConnectionPool
//////////////////////////////////////////////////////////////////////////

ConnectionPool* ConnectionPool::__instance = NULL;

ConnectionPool::ConnectionPool(const char* charset) : _driver(::sql::mysql::get_driver_instance()), _charset(NULL) { setCharset(charset); }

ConnectionPool::ConnectionPool(const LoginInfo& login_info, const char* charset) : _driver(::sql::mysql::get_driver_instance()), _login_info(login_info), _charset(NULL) { setCharset(charset); }

ConnectionPool::~ConnectionPool() { clear(); setCharset(NULL); }

Connection ConnectionPool::getConnection() {
	while (true) {
		Connection conn;
		{
			MutexGuard lock(_queue_mutex);
			if (_queue.empty())
				break;
			conn = _queue.front();
			_queue.pop();
		}
		if (!conn->isClosed())
			return conn;
		// Auto release conn
	}

	// No Connections in the pool...
	// Create new one
	return _login_info.connect(_charset);
}

void ConnectionPool::returnConnection(Connection conn) {
	MutexGuard lock(_queue_mutex);
	_queue.push(conn.release());
}

void ConnectionPool::clear() {
	while (true) {
		Connection conn;
		{
			MutexGuard lock(_queue_mutex);
			if (_queue.empty())
				return;
			conn = _queue.front();
			_queue.pop();
		}
		// Auto release conn
	}
}

void ConnectionPool::setCharset(const char* charset) {
	if (_charset != NULL) {
		delete [] _charset;
		_charset = NULL;
	}
	if (charset != NULL) {
		size_t len = strlen(charset);
		_charset = new char[++len];
		memcpy(_charset, charset, len);
	}
}

ConnectionPool* ConnectionPool::getInstance() {
	if (__instance == NULL)
		__instance = new ConnectionPool;
	return __instance;
}

void ConnectionPool::unload() {
	if (__instance != NULL) {
		delete __instance;
		__instance = NULL;
	}
}
