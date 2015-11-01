#pragma once
extern "C" 
{
#include "oftp/ftp_permission.h"
#include "oftp/oftpd.h"
#include "oftp/ftp_listener.h"

};

#include <string>
#include <vector>

namespace yiqiding { namespace net {

class KTVFtp
{
	ftp_listener_t t;
	std::vector<ftp_user_t *> _users;
public:
	void start(int number = 10);
	//dir as E:/test/
	void adduser(int permission , const std::string &username ,
		const std::string &pwd , const std::string &dir , const std::string &root);
	void stop();
};

}}