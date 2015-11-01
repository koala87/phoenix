/** telnet server
 * @author YuChun Zhang
 * @date 2014.04.03
 */
#include "net/TCP.h"
#include "Thread.h"
#include "stdint.h"

#pragma once

namespace yiqiding{namespace net{ namespace tel{


	const uint16_t MAX_MODNAME_LEN = MAX_PATH;
	const uint16_t MAX_MOD_NUM = 20;

	const uint8_t MAX_COMMAND_LENGTH = 100;
	const char NEWLINE_CHAR = '\n';
	const char BACKSPACE_CHAR = 8;
	const char BLANK_CHAR = ' ';
	const char RETURN_CHAR = 13;
	const char TAB_CHAR = 9;
	const char DEL_CHAR = 127;
	const char CTRL_S = 19;
	const char CTRL_R = 18;
	const char ARROW = 27;
//	const char UP_ARROW = 27;
//	const char DOWN_ARROW = 28;
//	const char LEFT_ARROW = 29;
//	const char RIGHT_ARROW = 30;

	enum PROMTSTATE
	{
		PROMTUSER	 = 1,
		PROMTPWD	 = 2,
		PROMTAUTHORIZED	 = 3,
	}; 
	
	enum DIRSTATE
	{
		STATE_E	=	0,		//方向键未开始
		STATE_B =   1,		//方向键开始
		STATE_N =   2,		//方向键确定 '['
	};
	
	#define TELCMD_WILL    (uint8_t)251
	#define TELCMD_WONT    (uint8_t)252
	#define TELCMD_DO      (uint8_t)253
	#define TELCMD_DONT    (uint8_t)254
	#define TELCMD_IAC     (uint8_t)255

	#define AUTHORIZATION_NAME_SIZE 20

	#define TELOPT_ECHO     (uint8_t)1
	#define TELOPT_SGA      (uint8_t)3
	#define TELOPT_LFLOW    (uint8_t)33
	#define TELOPT_NAWS     (uint8_t)34
	
	class ServerSend
	{
	public:
		virtual bool teleSend(const void *data , int len)=0;
			// '\n' must be '\r\n'
		virtual bool teleSend(const std::string &str)=0;

		virtual bool Send(const std::string &str) = 0;
	};
	typedef int (* ExeFunc)(void *ptr , ServerSend *telNet, int para1, int para2, int para3, int para4, int para5, int para6, int para7, int para8, int para9);

	class TelNet:public ServerSend
	{
	protected:
		struct TRawPara
		{
			char *paraStr;
			BOOL bInQuote;
			BOOL bIsChar;
		};

		struct FUNCInfo
		{
			ExeFunc			_cmdFunc;
			std::string		_name;
			std::string		_des;
			void			*ptr_;
			FUNCInfo( const std::string name ,const ExeFunc cmdFunc , const std::string des ,  void *ptr = NULL)
			{
				_cmdFunc = cmdFunc;
				_name = name;
				_des = des;
				ptr_ = ptr;
			}

		};
		std::string _promtname;
		tcp::Client _client;
		enum tel_state { tel_normal = 0, tel_nego = 1, tel_sub = 2 };
		int32_t seen_iac;
		tel_state state;
		int32_t count_after_sb;
		
		PROMTSTATE _promtstate; 
		std::string _username;
		std::string _password;
		bool		_usernamepass;

		char command[MAX_COMMAND_LENGTH];
		uint8_t cmdLen;

		
		DIRSTATE _direct;
		std::vector<std::string> _cmdlst;	
		int		_cmdindex;	

		int _checktimes;

		std::vector<FUNCInfo> _funclst;
		
		int8_t removeIAC(uint8_t c);
		
			// ''
		void promptShow();
			// 'word'
		void checkAuthorization(const std::string &value);
			//
		void sendIAC(uint8_t cmd, char opt);

		void RunCmd( char *szCmd);
		void CmdParse( char *szCmd , uint32_t len);
		int WordParse(const char * word);

		std::vector<std::string> findFunc(const std::string &prefix);
		FUNCInfo findFirstFunc(const std::string &username);
	
		void doAccept( tcp::Client &c);
		void doRead();	
		public:
		TelNet():_promtstate(PROMTUSER) , cmdLen(0),_usernamepass(false),seen_iac(0),count_after_sb(0),state(tel_normal),_promtname("telnet") ,_username("admin") ,_cmdindex(0),_checktimes(0) ,_direct(STATE_E) { }

	
		void setUserName(const std::string &usrname){ _username = usrname; }
		void setPassWord(const std::string &pwd) { _password = pwd; }
		void setPromtName(const std::string &promtname){ _promtname = promtname; }
		void addFunc(const std::string &funcname , ExeFunc func , const std::string &des ,  void *ptr = NULL){ _funclst.push_back(FUNCInfo(funcname , func , des , ptr)); }
		~TelNet(){}
		////@TeleSend API/////////////////////
		virtual	bool teleSend(const void *data , int len);
		// '\n' must be '\r\n'
		virtual	bool teleSend(const std::string &str);	

		virtual bool Send(const std::string &str)  { if(_promtstate != PROMTAUTHORIZED) return false; return teleSend(str + "\r\n");}
	};




	class TelNetServer : private tcp::Server , private Thread , public TelNet
	{
	private:
		bool _is_running;
	protected:	
		virtual void run();
	public:
		void Start(int port , int backlog = 5) { if(_is_running) return;  listen(port , backlog);_is_running = true; start();}
		void Stop(){ if(!_is_running) return ; _is_running = false; join();}
		virtual ~TelNetServer();
		TelNetServer():_is_running(false) {}
	};
}}}