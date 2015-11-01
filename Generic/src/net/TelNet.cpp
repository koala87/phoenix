/** telnet server
 * @author YuChun Zhang
 * @date 2014.04.03
 */
#include "net\TelNet.h"
#include <iostream>
#include <exception>
#include <string>
#include <sstream>

using namespace yiqiding::net::tel;
using namespace yiqiding::net;
int8_t TelNet::removeIAC(uint8_t c)
{
	int8_t ret = 0;
	if ((c == 255) && !seen_iac)    /* IAC*/
	{
		seen_iac = 1;
		return ret;
	}

	if (seen_iac)
	{
		switch(c)
		{
		case 251:
		case 252:
		case 253:
		case 254:
			if (state != tel_normal) {
				printf(" illegal negotiation.\n"); 
			}
			state = tel_nego;
			break;
		case 250:
			if (state != tel_normal){
				printf(" illegal sub negotiation.\n"); 
			}
			state = tel_sub;
			count_after_sb = 0;
			break;
		case 240:
			if (state != tel_sub) {
				printf(" illegal sub end.\n"); 
			}
			state = tel_normal;
			break;
		default:
			if (!((c > 240) && (c < 250) && (state == tel_normal)))
			{
				printf("illegal command.\n"); 
			}
			state = tel_normal;
		}
		seen_iac = 0;
		return 0;
	}

	switch (state)
	{
	case tel_nego:
		state = tel_normal;
		break;
	case tel_sub:
		count_after_sb++; /* set maximize sub negotiation length*/
		if (count_after_sb >= 100) state = tel_normal;
		break;
	default:
		ret = c;
	}
	return ret;

}

bool TelNet::teleSend(const void *data , int len)
{
	int ret = 0;
	int index = 0;
	int count = 3;
	if (_client.native() == INVALID_SOCKET ||data == NULL || len == 0)
		return false;
	

	do
	{
		ret = send(_client.native() , (char *)data + index , len , 0);
		if (ret == -1)
		{
			printf("teleSend err:%d\n" , GetLastError());
			_client.close();
			break;	
		}
		else if (ret == 0)
		{
			printf("teleSend net busy\n" , GetLastError());
			Sleep(50);	
		}
		else{
			index += ret;
		}
		count--;
	}while(count && index < len);
	
	return index == len;
}

bool TelNet::teleSend(const std::string &str)
{
	return teleSend(str.c_str() , (int)str.length());
}

void TelNet::promptShow()
{
	switch(_promtstate)
	{
	case PROMTUSER:
		teleSend("Username:");
		break;
	case PROMTPWD:
		teleSend("Password:");
		break;
	case PROMTAUTHORIZED:
		{
			std::ostringstream out;
			out<<_promtname <<"->";
			teleSend(out.str());			
		}
		break;
	default:
		printf("promptShow switch default error\n");
	}
}

void TelNet::checkAuthorization(const std::string &value)
{
	switch(_promtstate)
	{
	case PROMTUSER:
		_usernamepass = (value == _username);
		_promtstate = PROMTPWD;
		break;
	case PROMTPWD:
		_promtstate = (_usernamepass && (value == _password)) ? PROMTAUTHORIZED:PROMTUSER;
		if (_promtstate == PROMTUSER) _checktimes++;
		break;
	default:
		break;		
	}
	if (_checktimes >= 3)
	{
		_client.close();
	}
}

void TelNet::sendIAC(uint8_t cmd, char opt)
{	
	uint8_t buf[3];
	buf[0] = TELCMD_IAC;
	buf[1] = cmd;
	buf[2] = opt;
	teleSend(buf, 3);	
}

void TelNet::RunCmd( char *szCmd)
{
	int ret = 0;
	ExeFunc cmdFunc;
	std::string tmpcmd = szCmd;
	char *cmd = szCmd;
	int para[10];	
	TRawPara atRawPara[10];
	int paraNum = 0;
	uint8_t count = 0;
	uint8_t chStartCnt = 0;
	BOOL bStrStart = FALSE;
	BOOL bCharStart = FALSE;
	uint32_t cmdLen = (uint32_t)strlen(szCmd)+1;
	FUNCInfo funcinfo("" , NULL , "" );

	memset(para, 0, sizeof(para));
	memset(atRawPara, 0, sizeof(TRawPara)*10);
    switch(_promtstate)
    {
    case PROMTUSER:
    case PROMTPWD:
        {
            checkAuthorization(szCmd);
            break;
        }
    case PROMTAUTHORIZED:
        {
        
	        /* 解析参数、命令 */
	        while( count < cmdLen )
	        {	
		        switch(szCmd[count])
		        {
		        case '\'':
			        szCmd[count] = '\0';
			        if(!bCharStart)
			        {
				        chStartCnt = count;
			        }
			        else
			        {
				        if(count > chStartCnt+2)
				        {
					        teleSend( "input error.\r\n");
					        return;
				        }
			        }
			        bCharStart = !bCharStart;
			        break;

		        case '\"':
			        szCmd[count] = '\0';
			        bStrStart = !bStrStart;
 			        break;

		        case ',':
		        case ' ':
		        case '\t':
		        case '\n':
		        case '(':
		        case ')':
                    if( ! bStrStart )
			        {
				        szCmd[count] = '\0';
			        }
			        break;

		        default:
			        /* 如果本字符为有效字符，前一字符为NULL，表示旧单词结束，
			           新单词开始 */
			        if(count > 0 && szCmd[count-1] == '\0')
			        {				
				        atRawPara[paraNum].paraStr = &szCmd[count];
				        if(bStrStart)
				        {
					        atRawPara[paraNum].bInQuote = TRUE;
				        }
				        if(bCharStart)
				        {
					        atRawPara[paraNum].bIsChar = TRUE;
				        }
				        if(++paraNum >= 10)
					        break;
			        }
		        }
		        count++;
	        }

	        if(bStrStart || bCharStart)
	        {
		        teleSend("input error.\r\n");
		        return;
	        }

	        for(count=0; count<10; count++)
	        {
		        if(atRawPara[count].paraStr == NULL)
		        {
			        para[count] = 0;
			        continue;
		        }

		        if(atRawPara[count].bInQuote)
		        {
			        para[count] = (int)atRawPara[count].paraStr;
			        continue;
		        }

		        if(atRawPara[count].bIsChar)
		        {
			        para[count] = (char)atRawPara[count].paraStr[0];
			        continue;
		        }

		        para[count] = WordParse(atRawPara[count].paraStr);
	        }

	        /* 先执行命令 */
            if ( strcmp("bye", cmd) == 0 )
            {
                teleSend("\r\n  bye......\r\n");
				Sleep(500);
				_client.close();
                _promtstate = PROMTUSER;
		        return;
            }

			if( strcmp("man" , cmd) == 0)
			{
				funcinfo = findFirstFunc(atRawPara[0].paraStr);
				std::ostringstream out;
				if (funcinfo._name == "")
				{ 
					out << "can't find the cmd:" << atRawPara[0].paraStr << "\r\n";
				}
				else
				{
					out << atRawPara[0].paraStr << ":\r\n\t" <<funcinfo._des << "\r\n" ;
				}
				teleSend(out.str());
				return;
			}
			if (strcmp("help" , cmd) == 0)
			{
				std::vector<FUNCInfo>::iterator it = _funclst.begin();
				while(it != _funclst.end())
				{
					std::ostringstream out;
					out << it->_name <<":\r\n\t"<<it->_des << "\r\n";
					teleSend(out.str());
					it++;
				}
				return;
			}
			funcinfo = findFirstFunc(cmd);			  
			cmdFunc = funcinfo._cmdFunc;
			
			if(cmdFunc != NULL)
		    {
			    ret = cmdFunc(funcinfo.ptr_,this, para[0], para[1], para[2], para[3], para[4], para[5], para[6], para[7], para[8]);
			    std::ostringstream out;
				out << "\r\nvalue" << ret << "\r\n";
				teleSend(out.str());
				_cmdlst.push_back(tmpcmd);
				_cmdindex = (int)_cmdlst.size() - 1;
			    return;
		    }
            
			std::ostringstream out;
			out << "function "<<cmd <<" doesn't exist!\r\n";
            teleSend(out.str());
            break;
        }
    }
    return;
}

void TelNet::CmdParse( char *pchCmd , uint32_t byCmdLen)
{
	uint8_t count;
	int nCpyLen = 0;
	char command[MAX_COMMAND_LENGTH];
	if(byCmdLen > 0)
	{    
		//去头
		for(count=0; count<byCmdLen; count++)
		{
			char chTmp;

			chTmp = pchCmd[count];
			if(IsCharAlphaNumeric(chTmp)|| IsCharLower(chTmp) || IsCharUpper(chTmp))
			{
				break;
			}
		}

		nCpyLen = byCmdLen-count;
	}
	if(nCpyLen <= 0)
	{
		checkAuthorization("");                    
		return;
	}

	memcpy(command, pchCmd+count, nCpyLen);   
	if(byCmdLen < MAX_COMMAND_LENGTH)
	{
		command[nCpyLen] = '\0';
	}
	else
	{
		command[MAX_COMMAND_LENGTH-1] = '\0';
	}

	RunCmd(command);
}

int TelNet::WordParse(const char * word)
{
	
	int tmp;

	if(word == NULL) return 0;

	tmp = atoi(word);
	if(tmp == 0 && word[0] != '0')
	{
		return (int)word;
	}
	return tmp;
	
}


std::vector<std::string> TelNet::findFunc(const std::string &prefix)
{
	std::vector<std::string> vec;
	std::vector<FUNCInfo>::iterator it = _funclst.begin();
	while(it != _funclst.end())
	{
		if (it->_name.length() >= prefix.length() && memcmp(it->_name.c_str() , prefix.c_str() , prefix.length()) == 0)
		{
			vec.push_back(it->_name);
		}
		it++;
	}

	return vec;
}

TelNet::FUNCInfo TelNet::findFirstFunc(const std::string &username)
{
	FUNCInfo info("" ,NULL , "");
	
	std::vector<TelNet::FUNCInfo>::iterator it = _funclst.begin();
	while(it != _funclst.end())
	{
		if (it->_name == username)
		{
			info = *it;
			break;
		}
		it++;
	}
	return info;

}

void TelNet::doAccept( tcp::Client &c)
{
	_client = c;
	/* 设置TELE属性，打印欢迎语句*/
	sendIAC(TELCMD_DO, TELOPT_ECHO);
	sendIAC(TELCMD_DO, TELOPT_NAWS);
	sendIAC(TELCMD_DO, TELOPT_LFLOW);
	sendIAC(TELCMD_WILL, TELOPT_ECHO);
	sendIAC(TELCMD_WILL, TELOPT_SGA);

	/* 输出欢迎画面 */
	teleSend("*===============================================================\r\n");
	teleSend("Welcome To Lohas Telnet Server。\r\n");
	teleSend("*===============================================================\r\n");
	_promtstate = PROMTUSER;

	_direct = STATE_E;
	_cmdindex = 0;
	_checktimes = 0;
	_cmdlst.clear();
	cmdLen = 0;
	memset(command , 0 , sizeof(command));

	promptShow(); 
}

void TelNet::doRead()
{

	char cmdChar;

	int nRet;
	nRet = recv(_client.native() , &cmdChar , 1 , 0);
	if (nRet == 0)
	{
		_client.close();
		return;
	}

	cmdChar = removeIAC(cmdChar);


	if (_direct != STATE_E && _promtstate == PROMTAUTHORIZED)
	{
		
		if (cmdChar == '[' && _direct == STATE_B)
		{
			_direct = STATE_N;
		}
		else if(_direct == STATE_N)
		{
			if (cmdChar == 'A')
			{
				++_cmdindex;
				if (_cmdindex > _cmdlst.size() - 1)
				{
					_cmdindex = (int)_cmdlst.size() - 1;
				}
			}
			else if(cmdChar == 'B')
			{	
				--_cmdindex;
				if (_cmdindex < 0)
				{
					_cmdindex = 0;
				}
			}
			else if (cmdChar == 'C' || cmdChar == 'D')
				;
			else
			{
				teleSend("input error\r\n");
				promptShow();
				memset(command , 0 , sizeof(command));
				cmdLen = 0;
				_direct = STATE_E;
				return;
			}
			if(_cmdlst.size() != 0)
			{

				if(cmdLen > 0)
				{
					memset(command ,BACKSPACE_CHAR , cmdLen);
					teleSend(command, cmdLen);
					memset(command , BLANK_CHAR , cmdLen);
					teleSend(command , cmdLen);
					memset(command ,BACKSPACE_CHAR , cmdLen);
					teleSend(command, cmdLen);
				}
				memset(command , 0 , sizeof(command));
				cmdLen = (uint8_t)_cmdlst[_cmdindex].length();	
				//because the command saved , so the length must less than sizeof(command);  
				memcpy(command , _cmdlst[_cmdindex].c_str() , cmdLen);
				teleSend(command , cmdLen);
			}
			_direct = STATE_E;
		
		}
		else	//error
		{
			teleSend("input error\r\n");
			promptShow();
			memset(command , 0 , sizeof(command));
			cmdLen = 0;
			_direct = STATE_E;
		}
		return;
	}

		


	switch(cmdChar)
	{
	case CTRL_S:
		promptShow();
		break;
	case CTRL_R:
		promptShow();
		break;

	case RETURN_CHAR:         // 回车符
		teleSend("\r\n", 2); 
		CmdParse(command, cmdLen);
		cmdLen = 0;           
		memset(command,0,MAX_COMMAND_LENGTH);
		promptShow();         // 显示提示符
		break;

	case NEWLINE_CHAR:		/* 换行符 */		
		break;
	case ARROW:            // 方向箭头
		_direct = STATE_B;
		break;
//	case DOWN_ARROW:          // 下箭头
//	case LEFT_ARROW:          // 左箭头
//	case RIGHT_ARROW:         // 右箭头
		break;
	case TAB_CHAR:			//tab
		{
			if(_promtstate != PROMTAUTHORIZED || cmdLen == 0)
				break;
			command[cmdLen] = 0;
			std::vector<std::string> strlst = findFunc(command);
			int size = (int)strlst.size();
			if (size == 1)	//只有一个,就补充完整
			{
				if(strlst[0].length() > cmdLen && strlst[0].length() < MAX_COMMAND_LENGTH)
				{
					std::string suffix = strlst[0].substr(cmdLen);
					teleSend(suffix);
					memcpy(command , strlst[0].c_str() , strlst[0].length());
					cmdLen = (uint8_t)strlst[0].length();
				}
			}
			else if(size > 1)
			{
				teleSend("\r\n");
				int i;
				for ( i = 1 ; i <= size ; i++)
				{
					teleSend(strlst[i -1]);
					teleSend("\r\n");	
				}
			
				cmdLen = 0;           
				memset(command,0,MAX_COMMAND_LENGTH);
				promptShow();
			}	
		}
		break;

	case BACKSPACE_CHAR:         // 退格键
		if(cmdLen <= 0)
		{
			break;//???continue;
		}

		cmdLen--;	
		if(cmdLen >= 0 && cmdLen < MAX_COMMAND_LENGTH )
		{            
			command[cmdLen] = '\0';
		}
		if(_promtstate != PROMTPWD)
		{            
			/* 使光标后退，用一个空格擦除原字符，再使光标后退 */
			char tmpChar[3];

			tmpChar[0] = BACKSPACE_CHAR;
			tmpChar[1] = BLANK_CHAR;
			tmpChar[2] = BACKSPACE_CHAR;
			teleSend(tmpChar, 3);
		}
		break;

	default:
		/* add to command string */
		if(_promtstate != PROMTPWD)
		{            
			teleSend(&cmdChar, 1);
		}
		if(cmdLen < MAX_COMMAND_LENGTH)
		{				
			command[cmdLen++] = cmdChar;	
		}
		else
		{                
			teleSend("\r\n");

			CmdParse(command, cmdLen);	

			promptShow();         // 显示提示符
			cmdLen = 0;
			memset(command,0,MAX_COMMAND_LENGTH);
		}
		break;
	}

}


void TelNetServer::run()
{
	FD_SET fd;
	int nRet;
	
	while(_is_running)
	{
		try{
	  
			FD_ZERO(&fd);
			FD_SET(native() , &fd);
			if (_client.native() != INVALID_SOCKET)
			{
				FD_SET(_client.native() , &fd);
			}
			timeval tv;
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			nRet = select(0 , &fd , NULL , NULL , &tv);
			if (nRet == 0)
			{
				continue;;
			}
			else if(nRet == -1)
			{
				break;
			}
	
			if(FD_ISSET(native() , &fd))
			{
				doAccept(accept());	
			}
			else if (FD_ISSET(_client.native() , &fd))
			{	
				doRead();
			}
		}catch(const std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

TelNetServer::~TelNetServer()
{
	Stop();
}