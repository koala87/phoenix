/*
* SmtpMail smtp protocol client
* @author Yuchun Zhang 
* @date 2014.08.06
*/

#include "net/smtpmail.h"
#include "base64.h"
#include "io/File.h"
#include "time.h"


using namespace yiqiding::net::mail;
using namespace yiqiding::net::tcp;
using namespace yiqiding::io;

SmtpMail::SmtpMail(MailType type):_ssl_client_ctx(NULL),_ssl(NULL),_type(type),_state(MAIL_INIT)
{
	if(_type == MAIL_SECURE)
	{	
		SSL_library_init();
		_ssl_client_ctx = SSL_CTX_new(SSLv23_client_method());
	}
}

bool SmtpMail::Send(const char *data , int len)
{
	int result = 0;
	int index = 0;
	if(_type == MAIL_NORMAL)
	{
		_c.write(data , len);
	}
	else
	{
		index = SSL_write(_ssl , data + result , len - result);
		if(index <= 0)
			return false;
		result += index;
		
		if(result < len)
		{	
			do{
				Sleep(3);
				index = SSL_write(_ssl , data + result , len - result);
				if(index <= 0)
					return false;
				result += index;
			}while(result < len);
		}
	}
	return true;
}

bool SmtpMail::Talk( const std::string &ret)
{
	char buff[1024];

	int n; 
	if(_type == MAIL_NORMAL)
	 n = _c.readOnce(buff , 1024);
	else
	 n = SSL_read(_ssl , buff , 1024);
	if(n >= 1024 || n <= 3)
		return false;
	if(strncmp(buff , ret.c_str() , ret.length()) == 0)
		return true;
	return false;

}

bool SmtpMail::Talk( const std::string &data , const std::string &ret)
{
	char buff[1024];
	int n;
	if(!Send(data.c_str() , data.length()))
		return false;
	if(_type == MAIL_NORMAL)
		n = _c.readOnce(buff , 1024);
	else
		n = SSL_read(_ssl , buff , 1024);
	if(n >= 1024 || n <= 3)
		return false;
	if(strncmp(buff , ret.c_str() , ret.length()) == 0)
		return true;
	return false;

}

bool SmtpMail::Login(const std::string &usr , const std::string &pwd , const std::string &host , int port)
{
	_usr = usr;
	_pwd = pwd;
	_host = host;
	_port = port;

	if(_state != MAIL_INIT)
		return false;

	try
	{
		Connect();

		if(!Talk("220"))
			return false;

		//if(!Talk( "EHLO "+ host +"\r\n" , "250"))
		//	return false;

		if(!Talk( "HELO " + host + "\r\n" , "250"))
			return false;

		if(!Talk( "AUTH LOGIN\r\n" , "334"))
			return false;

		char base64[1024];
		if(!Talk((Base64_Encode(base64 , usr.c_str() , usr.length()) , std::string(base64)) + "\r\n" , "334"))
			return false;

		if(!Talk(( Base64_Encode(base64 , pwd.c_str() , pwd.length()) ,std::string(base64)) + "\r\n" , "235" ))
			return false;
		
		_state = MAIL_LOGIN;
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool SmtpMail::Connect()
{
	
	_c.connect(_host , _port);
	if(_type != MAIL_SECURE)
		return true;

	_ssl = SSL_new(_ssl_client_ctx);
	if(!_ssl)
		return false;
	SSL_set_fd(_ssl , _c.native());
	int ret = SSL_connect(_ssl);
	if(ret != 1)
		return false;
	X509 *server_cert = SSL_get_peer_certificate(_ssl);
	if(server_cert == NULL)
		return false;
	X509_free(server_cert);
	return true;
}

bool SmtpMail::sendBody(const std::vector<std::string> &tos, const std::vector<std::string> &ccs, const std::vector<std::string> &bccs , const std::string &subject , const std::string &body)
{
	_tos = tos;
	_ccs = ccs;
	_bccs = bccs;
	_subject = subject;
	_body = body;
	int n;
	if(_state != MAIL_LOGIN)
		return false;

	if(!Talk("MAIL FROM:<" + _usr + ">\r\n" , "250"))
		return false;
	
	std::string to;
	for each( auto s in tos)	
	{
		to = "RCPT TO:<"+ s + ">\r\n";
		if(!Talk(to , "250"))
		return false;
	}

	for each(auto s in ccs)
	{
		to = "RCPT TO:<"+ s + ">\r\n";
		if(!Talk(to , "250"))
			return false;
	}
	
	for each(auto s in bccs)
	{
		to = "RCPT TO:<"+ s + ">\r\n";
		if(!Talk(to , "250"))
			return false;
	}
	

	if(!Talk("DATA\r\n" , "354"))
		return false;

	to = "from:"+_usr+"<" + _usr + ">\r\n";
	to += "to:";
	int index = 0;
	for each(auto s in tos)
	{	
		if(index != 0)
			to += ",";
		to +=  "<" +  s + ">" ;
		++index;

	}
	to += "\r\n";
	if(!ccs.empty())
	{
		index = 0;
		to += "cc:";
		for each(auto s in ccs)
		{	
			if(index != 0)
				to += ",";
			to +=  "<" + s + ">";
			++index;
		}
		to += "\r\n";
	}

	if(!bccs.empty())
	{
		index = 0;
		to += "bcc:";
		for each(auto s in bccs)
		{	
			if(index != 0)
				to += ",";
			to +=  "<" + s + ">";
			++index;
		}	
		to += "\r\n";
	}

	//time
	{
		time_t lt =	::time(NULL);
		struct tm ptm;
		gmtime_s(&ptm , &lt);
		char mytime[100];
		strftime(mytime , 100 , "Date: %a, %d %b %Y %H:%M:%S\r\n" , &ptm);
		to += mytime;
	}

	to += "MIME-VERSION: 1.0\r\nsubject:" + subject + "\r\n" +  "Content-type:multipart/mixed;Boundary=lohas\r\n\r\n";
	
	//body
	to += "--lohas\r\n";
	to += "Content-type:text/plain;Charset=utf-8\r\n";
	to += "Content-Transfer-Encoding:8bit\r\n\r\n";
	to += body + "\r\n\r\n";	
	if(!Send(to.c_str() , to.length()))
		return false;
	_state = MAIL_BODY;

	return true;

}

bool SmtpMail::sendEnd()
{
	std::string to = "--lohas--\r\n.\r\n";
	if(!Talk(to , "250"))
		return false;

	if(!Talk("QUIT\r\n", "221"))
		return false;

	return true;
}

bool SmtpMail::sendAttach(const char *data , int len , const std::string &name)
{
	if(_state != MAIL_BODY)
		return false;
	std::string to = "--lohas\r\n";
	char *new_data = new char[len * 4 / 3 + 4];
	int n = 0;
	Base64_Encode(new_data , data , len);
	to += "Content-Type:application/octet-stream;Name="+name+"\r\n";
	to += "Content-Disposition:attachment;FileName="+name+"\r\n";
	to += "Content-Transfer-Encoding:Base64\r\n\r\n";
	to += new_data;
	to += "\r\n\r\n";
	if(!Send(to.c_str() , to.length()))
		return false;
	delete new_data;
	return true;
}

bool SmtpMail::sendBody(const std::string &to , const std::string &subject , const std::string &body)
{
	std::vector<std::string> tos;
	tos.push_back(to);
	std::vector<std::string> empty;
	return sendBody(tos , empty ,empty , subject , body);

}

bool SmtpMail::sendAttach(const std::string &path , const std::string &name , int len)
{
	try {
		File fp(path);
		fp.open(File::READ);
		DWORD high,low;
		LONG hHigh;
		int pos = 0;
		low = GetFileSize(fp.native() , &high);
		if(low > len) pos = low - len; 
		else len = low;
		std::auto_ptr<char> data;
		data.reset(new char[len]);
		SetFilePointer(fp.native() , pos , &hHigh , FILE_CURRENT);
	
		//取一行,避免GBK两字节导致的截断
		{
			char c;
			do{
				fp.read(&c , 1);
				--len;
				if(len == 0)
				{
					return false;
				}
			}while(c != '\n');
				
		}

		fp.read(data.get() , len);
		if(!sendAttach(data.get() , len , name))
		{
			return false;
		}

		return true;
	} catch (...) {
		return false;
	}
}

bool SmtpMail::sendAttach(const std::string &path , const std::string &name)
{

	char *data = NULL;
	try{
		File fp(path);
		fp.open(File::READ);
		DWORD high,low;
		low = GetFileSize(fp.native() , &high);
		data = new char[low];
		fp.read(data , low);
		if(!sendAttach(data , low , name))
		{
			delete data;
			return false;
		}
		delete data;
		return true;
	}
	catch(...)
	{
		if(data != NULL)
			delete data;
		return false;
	}
}