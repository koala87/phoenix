/**
 * Curl Distribute Files
 * Lohas Network Technology Co., Ltd
 * @author Yuchun zhang
 * @date 2014.08.15
 */
#pragma once
#include <vector>
#include "curl/curl.h"
#include "Thread.h"
#include "net/DynamicData.h"
#include <map>
#include "container/SmartVector.h"

namespace yiqiding { namespace net {

	const int CURL_LOW_SPEED_LIMIT = 64 * 1024;

	const int CURL_LOW_SPEED_TIME = 8;

	struct ProgressData
	{
		double totalToDownload;
		double nowDownloaded;
		double totalToUpLoad;
		double nowUpLoaded;
	};
	
	const std::string yiqichang_server = "http://static.yiqiding.com/ktvlog";
	const std::string yiqichang_server2 = "http://static.yiqiding.com/ktvstatistics";
	//const std::string yiqichang_data= "http://115.159.85.203:8080/luoha";
	class DistributeProgress
	{
	private:
		std::map<void * , ProgressData> _vecHost;
		int								_size;
		Mutex							_mutex;
		double							_nowUpLoaded;
	protected:
		//@ single thread
		virtual int process(double now , double total){  /*printf("\b\b\b\b\b\b\r%d%%" , int(now/total*100));*/ return 0;}
	public:
		void setSize(int size){ _size = size;}
		static int HandlerProgressFunc(void *ptr ,  double totalToDownload, double nowDownloaded, double totalToUpLoad, double nowUpLoaded)
		{

			bool complete = false;
			double inowUpLoaded = 0;
			double itotalToUpLoad = 0;
			void **data_ptr =(void**) ptr;
			DistributeProgress *progress = (DistributeProgress *)data_ptr[0];
			//printf("%d:r:%f/%f t:%f/%f\n" ,timeGetTime() ,nowDownloaded , totalToDownload , nowUpLoaded, totalToUpLoad );
			if(progress == NULL || (totalToUpLoad > -1e-6 && totalToUpLoad < 1e-6))
				return 0;
			ProgressData data = {totalToDownload ,nowDownloaded,totalToUpLoad,nowUpLoaded};
			
			{
				MutexGuard guard(progress->_mutex);
				progress->_vecHost[data_ptr[1]] = data;
				complete = progress->_size == progress->_vecHost.size();
				if(complete)
				{	
					for each(auto k in progress->_vecHost)
					{
						inowUpLoaded += k.second.nowUpLoaded;
						itotalToUpLoad += k.second.totalToUpLoad;
					}
					
					if(progress->_nowUpLoaded != inowUpLoaded)
					{
						progress->_nowUpLoaded = inowUpLoaded;
						return progress->process(inowUpLoaded , itotalToUpLoad);
					}
				}
				
			}
			return 0;
		}
		
		DistributeProgress():_nowUpLoaded(0){}
		virtual ~DistributeProgress(){}
	};

	class DistributeCurl
	{
	private:
		std::string _url;

		
	protected:
	public:
		virtual void* postFile(const std::string &localpath , const std::string &filename , const std::string &type ,const std::string &mime , DistributeProgress *progress = NULL );
		virtual bool  getResult(void* t, std::string &result);
		//call must after getResult
		virtual void  close(void *t);		

		// user api
		std::string & getUrl(){ return _url;};
		DistributeCurl(const std::string &url ):_url(url){};
		virtual ~DistributeCurl(){}

	};

	class DistributeThread:private Thread
	{
	private:
		std::string _host;
		DynamicData _data;
		std::string _filename;
		std::string _localpath;
		std::string	_type;
		std::string _mime;
		DistributeProgress	*_progress;
		DistributeThread(const std::string &host ,
		const std::string &filename ,
	 	const std::string &localpath ,
		const std::string &type,
		const std::string &mime,
		DistributeProgress *progress
		):_host(host),_filename(filename),_localpath(localpath),_type(type),_mime(mime),_progress(progress){};
		std::string getResult() { return std::string(_data.getData() , _data.getLenth());} 
	public:
		static size_t write_data(void * ptr, size_t size, size_t nmemb, void * stream)
		{
			DynamicData *data = (DynamicData *)stream;
			data->write((char *)ptr , size * nmemb);
			return size * nmemb;
		
		}

		static size_t write_file(void *ptr , size_t size , size_t nmemb , void *stream)
		{
			FILE *fp= ( FILE *)stream;
			return fwrite(ptr , size , nmemb , fp);
		}
	protected:
		void run();
		friend 	DistributeCurl;
  };

	class DistributeContent
	{
	private:
		static DistributeContent *_content;
		yiqiding::container::SmartVector<DistributeCurl> _vecHost;
		DistributeContent(){}
	public:
		bool DisFile(const std::string &localpath ,const std::string &filename , std::string &result ,const std::string type, const std::string mime = "application/octet-stream" , std::auto_ptr<DistributeProgress> progress = std::auto_ptr<DistributeProgress>(NULL));

		void addHost(DistributeCurl *host){ _vecHost.push_back(host); };
	
		static DistributeContent * getInstance(){if(_content == NULL) {curl_global_init(CURL_GLOBAL_ALL); _content = new DistributeContent();} return _content; };
		static void				   unLoad() { delete _content;}
		
		static bool UpLoadFile(
			const std::string &url,const std::string &filename,const std::string &filetype,
			const std::string &localpath ,const std::string &shopname, std::auto_ptr<DistributeProgress> process = std::auto_ptr<DistributeProgress>(NULL));  
	
		
		static bool UpLoadFile2(const std::string &url,const std::string &filename,const std::string &filetype,
			const std::string &localpath , std::auto_ptr<DistributeProgress> process = std::auto_ptr<DistributeProgress>(NULL));
	
	
		static bool UpLoadFile3(const std::string &filename, const std::string &localpath , std::auto_ptr<DistributeProgress> process = std::auto_ptr<DistributeProgress>(NULL));

		static bool UpLoadFile4(const std::string &url , const std::string &filename , const std::string &filetype ,
			const std::string &localpath , const std::string &shopname ,std::auto_ptr<DistributeProgress> process = std::auto_ptr<DistributeProgress>(NULL));

	};



} }





