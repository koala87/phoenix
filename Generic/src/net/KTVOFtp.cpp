#include "net/KTVOFtp.h"
#include "utility/Utility.h"
using namespace yiqiding::net;


void KTVFtp::start(int number )
{
	error_t err;
	ftp_listener_init(&t , NULL , NULL , MAX_CLIENTS , INACTIVITY_TIMEOUT , &err);

	init_user_manager(number);

	ftp_listener_start(&t , &err);
	
}

void KTVFtp::adduser(int permission ,const std::string &username ,const std::string &pwd , const std::string &dir ,const std::string &root)
{
	ftp_user_t *user = new ftp_user_t;
	user->permission = permission;
	std::string myDir = yiqiding::utility::replace(dir , "\\" , "/");
	std::string myRoot = yiqiding::utility::replace(root , "\\" , "/");
	if(myDir[myDir.length() - 1] != '/')
		myDir += "/";
	if (myRoot[myRoot.length() - 1] != '/')
		myRoot += "/";

	strcpy(user->password , pwd.c_str());
	strcpy(user->username , username.c_str());
	strcpy(user->dir , myDir.c_str());
	strcpy(user->root , myRoot.c_str());
	_users.push_back(user);
	add_user(user);
}

void KTVFtp::stop()
{
	ftp_listener_stop(&t);
	destroy_user_manager();
	for each(auto k in _users)
	{
		delete k;
	}
	_users.clear();
}