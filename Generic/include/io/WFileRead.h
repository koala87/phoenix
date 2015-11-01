/**  unicode file in readline end with 0x0d 0x0a and ,out utf8/gbk code
	@author Yuchun Zhang
	@date 2014.03.17
*/


#pragma  once
#include <vector>
#include "io/File.h"
#include "tility/Transcode.h"


namespace yiqiding { namespace io { 


	const int wfileHead = 2;
	class WFileRead
	{
	private:
		yiqiding::io::File		_fp;
		int						_index;
		char*					_rdbuf;
		int						_len;

		//返回<=0，表示失败，>0成功，表示unicode所占空间大小。
		int					getWstring(char * data ,const int len);
	public:
		WFileRead(const std::string &path) :_fp(path)
		{
			_rdbuf = NULL;
			_index = 0; 
			_len = 0;
		}
		~WFileRead() { close();}
		void open();
		void close() 
		{
			if (_rdbuf)
			{
				delete []_rdbuf;
				_rdbuf = NULL;
			}
			_fp.close();
		}
		void readLine(std::string &read , yiqiding::utility::CodePage::Charset code = yiqiding:: utility::CodePage::UTF8);
	};



}}
