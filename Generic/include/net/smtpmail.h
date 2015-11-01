/**
 * SmtpMail
 * @author Yuchun Zhang
 * @date 2014.08.06
 */

#pragma once

#include "net/TCP.h"
#include <string>
#include "openssl/ssl.h"


namespace yiqiding{ namespace net {namespace mail{
	

	enum MailState
	{
		MAIL_INIT,
		MAIL_LOGIN,
		MAIL_BODY,
		MAIL_END
	};

	enum MailType
	{
		MAIL_NORMAL,
		MAIL_SECURE,
	};


	class SmtpMail
	{
		yiqiding::net::tcp::Client	_c;
		std::string					_usr;
		std::string					_pwd;
		std::vector<std::string>	_tos;
		std::vector<std::string>	_ccs;
		std::vector<std::string>	_bccs;
		std::string					_host;
		int							_port;
		std::string					_subject;
		std::string					_body;
		MailState					_state;
		MailType					_type;
		SSL_CTX						*_ssl_client_ctx;
		SSL							*_ssl;
	public:

		~SmtpMail() {
			if(_ssl) 
			{
				SSL_shutdown(_ssl); 
				SSL_free(_ssl);
			}
			if(_ssl_client_ctx) 
				SSL_CTX_free(_ssl_client_ctx);
			
		}
		SmtpMail(MailType type = MAIL_NORMAL);
		bool Login(const std::string &usr , const std::string &pwd , const std::string &host , int port = 25);	
		bool sendBody(const std::vector<std::string> &tos, 
					const std::vector<std::string> &ccs,
				 const std::vector<std::string>	&bccs , 
				const std::string &subject ,
				const std::string &body);
		bool sendAttach(const char *data , int len , const std::string &name);
		bool sendEnd();


		bool sendBody(const std::string &to , const std::string &subject , const std::string &body);
		bool sendAttach(const std::string &path , const std::string &name);
		bool sendAttach(const std::string &path , const std::string &name , int len);
	private:
		bool  Talk(const std::string &data , const std::string &ret);
		bool  Talk(const std::string &ret);
		bool  Connect();
		bool  Send(const char *data , int len);
};

	

}}}
