#include "net/FileCurlDistribute.h"
#include "curl/curl.h"
#include "utility/Logger.h"
#include "utility/Utility.h"
#include "io/File.h"
#include "crypto/MD5.h"
using namespace yiqiding::net;



void DistributeThread::run()
{
	CURL * curl = curl_easy_init();

	static const char buf[] = "Expect:";  
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist	 *headerlist = NULL;
	void *progress_data[2];
	progress_data[0] = _progress;
	progress_data[1] = curl;

	headerlist = curl_slist_append(headerlist, buf);  
	
	curl_formadd(&formpost ,
		&lastptr , 
		CURLFORM_COPYNAME , "file",
		CURLFORM_FILE , _localpath.c_str(),
		CURLFORM_FILENAME, _filename.c_str(),
		CURLFORM_CONTENTTYPE, _mime.c_str(),
		CURLFORM_END
		);
	
	curl_formadd(&formpost ,
		&lastptr , 
		CURLFORM_COPYNAME , "type",
		CURLFORM_COPYCONTENTS , _type.c_str(),
		CURLFORM_END
		);



	curl_formadd(&formpost ,
		&lastptr , 
		CURLFORM_COPYNAME , "submit",
		CURLFORM_END
		);

	
	curl_easy_setopt(curl, CURLOPT_URL, _host.c_str());		
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 
	curl_easy_setopt(curl, CURLOPT_HTTPPOST , formpost);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_data);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);                
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DistributeProgress::HandlerProgressFunc);
	curl_easy_setopt(curl , CURLOPT_PROGRESSDATA,progress_data);
	curl_easy_setopt(curl , CURLOPT_LOW_SPEED_LIMIT , CURL_LOW_SPEED_LIMIT);
	curl_easy_setopt(curl , CURLOPT_LOW_SPEED_TIME , CURL_LOW_SPEED_TIME);

	CURLcode code =	curl_easy_perform(curl);
	if(code != CURLE_OK)
	{
		_data.reset();
		_data.write("0",1);
		yiqiding::utility::Logger::get("system")->log("curl not ok: " + yiqiding::utility::toString(code)  + " " + _localpath + " " + _filename +" "+_host ,yiqiding::utility::Logger::WARNING);
	}
	if(code == CURLE_OK)
	{
		long retcode;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
		if(retcode != 200)
		{
			_data.reset();
			_data.write("0",1);
			yiqiding::utility::Logger::get("system")->log("curl " + _localpath + " " + _filename +" "+_host + "code:" + yiqiding::utility::toString(retcode) ,yiqiding::utility::Logger::WARNING);	
		}
	}

	curl_easy_cleanup(curl);
	curl_formfree(formpost);  
	curl_slist_free_all (headerlist);  
}

void* DistributeCurl::postFile(const std::string &localpath , const std::string &filename  ,const std::string &type, const std::string &mime, DistributeProgress *progress)
{
	DistributeThread *thread = new DistributeThread(_url , filename , localpath  ,type, mime, progress );
	thread->start();
	return thread;
}

bool DistributeCurl::getResult(void* t , std::string &result)
{
	if(t == NULL)
		return false;
	DistributeThread *thread = (DistributeThread *)t;
	thread->join();
	result = thread->getResult();
	if(result == "0" || result == "1" || result == "")
		return false;
	return true;
}

void DistributeCurl::close(void *t)
{
	if(t == NULL)
		return;
	DistributeThread *thread = (DistributeThread *)t;
	delete thread;
}


DistributeContent * DistributeContent::_content = NULL;


bool DistributeContent::DisFile(const std::string &localpath ,const std::string &filename , std::string &result , const std::string type,const std::string mime , std::auto_ptr<DistributeProgress> progress)
{	

	if(!yiqiding::io::File::isExist(localpath))
	{
		yiqiding::utility::Logger::get("system")->log(localpath + " not exist " + filename ,yiqiding::utility::Logger::WARNING);
		return false;
	}
	void **threads = new void*[_vecHost.size()];
	std::string tmp;
	result = "";
	bool flag = true;
	int index = 0;	
	if(progress.get() != NULL)
	progress->setSize((int)_vecHost.size());
	
	for each( auto h in _vecHost)
		threads[index++] = h->postFile(localpath , filename  , type, mime,progress.get());
		
	index = 0;
	for each(auto h in _vecHost)
	{	//distribute error , may be del other's
		 if(!h->getResult(threads[index++] , tmp))
		 {
			 yiqiding::utility::Logger::get("system")->log(h->getUrl() +" result:" + tmp , yiqiding::utility::Logger::WARNING);
			 flag = false;
		 }
	}
	

	index = 0;
	for each(auto h in _vecHost)
		h->close(threads[index++]);

	delete threads;

	if(flag)	result = tmp;
	
	if(result.size() != 0 && result[0] != '/')
		result = "/" + result;
	return	flag;	

}


bool DistributeContent::UpLoadFile(
	const std::string &url,
	const std::string &filename,
	const std::string &filetype,
	const std::string &localpath ,
	const std::string &shopname,
	std::auto_ptr<DistributeProgress> process )
 {
	 CURL * curl = curl_easy_init();

	 bool flag = true;

	 DynamicData data;
	 static const char buf[] = "Expect:";  
	 struct curl_httppost *formpost = NULL;
	 struct curl_httppost *lastptr = NULL;
	 struct curl_slist	 *headerlist = NULL;
	 void *progress_data[2];
	 progress_data[0] = process.get();
	 progress_data[1] = curl;

	 headerlist = curl_slist_append(headerlist, buf);  

	 curl_formadd(&formpost ,
		 &lastptr , 
		 CURLFORM_COPYNAME , "logfile",
		 CURLFORM_FILE , localpath.c_str(),
		 CURLFORM_FILENAME, filename.c_str(),
		 CURLFORM_CONTENTTYPE, filetype.c_str(),
		 CURLFORM_END
		 );

	 curl_formadd(&formpost , 
		 &lastptr,
		 CURLFORM_COPYNAME, "shopname",
		 CURLFORM_COPYCONTENTS,shopname.c_str(),
		 CURLFORM_END
		 );


	 curl_formadd(&formpost ,
		 &lastptr , 
		 CURLFORM_COPYNAME , "submit",
		 CURLFORM_END
		 );


	 curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		
	 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 
	 curl_easy_setopt(curl, CURLOPT_HTTPPOST , formpost);
	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DistributeThread::write_data);
	 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	 curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);                
	 curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DistributeProgress::HandlerProgressFunc);
	 curl_easy_setopt(curl , CURLOPT_PROGRESSDATA,progress_data);
	 curl_easy_setopt(curl , CURLOPT_LOW_SPEED_LIMIT , CURL_LOW_SPEED_LIMIT/1024);
	 curl_easy_setopt(curl , CURLOPT_LOW_SPEED_TIME , CURL_LOW_SPEED_TIME);
	 curl_easy_setopt(curl , CURLOPT_CONNECTTIMEOUT_MS , 3000);

	 CURLcode code =	curl_easy_perform(curl);
	 if(code != CURLE_OK)
	 {
		 flag = false;
	 }
	 if(code == CURLE_OK)
	 {
		 long retcode;
		 curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
		 if(retcode != 200)
		 {
			 flag = false;
		 }
	 }

	 curl_easy_cleanup(curl);
	 curl_formfree(formpost);  
	 curl_slist_free_all (headerlist);  

	 return flag;
 }

 bool DistributeContent::UpLoadFile2(const std::string &url,const std::string &filename,const std::string &filetype,
	const std::string &localpath , std::auto_ptr<DistributeProgress> process){
	
		CURL * curl = curl_easy_init();

		bool flag = true;

		DynamicData data;
		static const char buf[] = "Expect:";  
		struct curl_httppost *formpost = NULL;
		struct curl_httppost *lastptr = NULL;
		struct curl_slist	 *headerlist = NULL;
		void *progress_data[2];
		progress_data[0] = process.get();
		progress_data[1] = curl;

		headerlist = curl_slist_append(headerlist, buf);  

		curl_formadd(&formpost ,
			&lastptr , 
			CURLFORM_COPYNAME , "data",
			CURLFORM_FILE , localpath.c_str(),
			CURLFORM_FILENAME, filename.c_str(),
			CURLFORM_CONTENTTYPE, filetype.c_str(),
			CURLFORM_END
			);


		curl_formadd(&formpost ,
			&lastptr , 
			CURLFORM_COPYNAME , "submit",
			CURLFORM_END
			);


		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 
		curl_easy_setopt(curl, CURLOPT_HTTPPOST , formpost);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DistributeThread::write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);                
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DistributeProgress::HandlerProgressFunc);
		curl_easy_setopt(curl , CURLOPT_PROGRESSDATA,progress_data);
		curl_easy_setopt(curl , CURLOPT_LOW_SPEED_LIMIT , CURL_LOW_SPEED_LIMIT/1024);
		curl_easy_setopt(curl , CURLOPT_LOW_SPEED_TIME , CURL_LOW_SPEED_TIME);
		curl_easy_setopt(curl , CURLOPT_CONNECTTIMEOUT_MS , 3000);

		CURLcode code =	curl_easy_perform(curl);
		if(code != CURLE_OK)
		{
			flag = false;
		}
		if(code == CURLE_OK)
		{
			long retcode;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
			if(retcode != 200)
			{
				flag = false;
			}
		}

		curl_easy_cleanup(curl);
		curl_formfree(formpost);  
		curl_slist_free_all (headerlist);  

		return flag;


 }

 

 bool DistributeContent::UpLoadFile3(const std::string &filename , const std::string &localpath , std::auto_ptr<DistributeProgress> process)
 {

	 std::string username = "song";
	 std::string pwd = "yiqiding1314";
	 std::string buckname = "yiqichang-song";
	 std::string api_url = "v0.api.upyun.com";

	 char time_str[15];
	 {
		 time_t now = time(NULL);
		 struct tm timeinfo;	 
		 localtime_s(&timeinfo, &now);
		 strftime(time_str,15,"%Y-%m-%d", &timeinfo);
	 }
	 std::string uri = "/" + buckname + "/" + time_str + "/" + filename ; 

	std::string date;
	{
		 char mytime[100];
		 time_t lt =	::time(NULL);
		 struct tm ptm;
		 gmtime_s(&ptm , &lt);
		 strftime(mytime , 100 , "%a, %d %b %Y %H:%M:%S GMT" , &ptm);
		 date = mytime;
	}
	 

	 bool flag = true;

	 int len = yiqiding::io::File::getLength(localpath);
	 yiqiding::crypto::MD5 md5("PUT&" + uri + "&" + date + "&" + yiqiding::utility::toString(len) + "&" + yiqiding::crypto::MD5(pwd).hex());
	 std::string sign = md5.hex();
	 std::string hexpert = "Expect:";
	 std::string hdate = "Date: " + date;
	 std::string hauth = "Authorization: UpYun " + username + ":" + sign;  
	 std::string hmdir = "mkdir: true";
	 struct curl_slist	 *headerlist = NULL;
	 CURL * curl = curl_easy_init();
	 headerlist = curl_slist_append(headerlist, hexpert.c_str());
	 headerlist = curl_slist_append(headerlist, hdate.c_str());
	 headerlist = curl_slist_append(headerlist, hauth.c_str());
	 headerlist = curl_slist_append(headerlist, hmdir.c_str());

	 FILE *fp = fopen(localpath.c_str() , "rb");
	 if(!fp)
	 {
		 yiqiding::utility::Logger::get("system")->log(localpath + " can not open" , yiqiding::utility::Logger::WARNING);
		 return false;
	 }
	 uri = "http://v0.api.upyun.com" + uri;


	 curl_easy_setopt(curl, CURLOPT_URL,   uri.c_str());		
	 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 
	 curl_easy_setopt(curl, CURLOPT_PUT , true);
	 curl_easy_setopt(curl , CURLOPT_INFILE , fp);
	 curl_easy_setopt(curl , CURLOPT_INFILESIZE , len);
	
	curl_easy_setopt(curl , CURLOPT_LOW_SPEED_LIMIT , CURL_LOW_SPEED_LIMIT/1024);
	curl_easy_setopt(curl , CURLOPT_LOW_SPEED_TIME , CURL_LOW_SPEED_TIME);
	curl_easy_setopt(curl , CURLOPT_CONNECTTIMEOUT_MS , 3000);

	 CURLcode code =curl_easy_perform(curl);
	 if(code != CURLE_OK)
	 {
		 flag = false;
	 }
	 if(code == CURLE_OK)
	 {
		 long retcode;
		 curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
		 if(retcode != 200)
		 {
			 flag = false;
		 }
	 }

	 curl_slist_free_all(headerlist);  
	 curl_easy_cleanup(curl);
	 fclose(fp);
	 return flag;
 }

  bool DistributeContent::UpLoadFile4(const std::string &url , const std::string &filename , const std::string &filetype ,
	 const std::string &localpath , const std::string &shopname ,std::auto_ptr<DistributeProgress> process)
 {

	 CURL * curl = curl_easy_init();

	 bool flag = true;

	 DynamicData data;
	 static const char buf[] = "Expect:";  
	 struct curl_httppost *formpost = NULL;
	 struct curl_httppost *lastptr = NULL;
	 struct curl_slist	 *headerlist = NULL;
	 void *progress_data[2];
	 progress_data[0] = process.get();
	 progress_data[1] = curl;

	 headerlist = curl_slist_append(headerlist, buf);  

	 curl_formadd(&formpost ,
		 &lastptr , 
		 CURLFORM_COPYNAME , "file",
		 CURLFORM_FILE , localpath.c_str(),
		 CURLFORM_FILENAME, filename.c_str(),
		 CURLFORM_CONTENTTYPE, filetype.c_str(),
		 CURLFORM_END
		 );

	 curl_formadd(&formpost , 
		 &lastptr,
		 CURLFORM_COPYNAME, "name",
		 CURLFORM_COPYCONTENTS,shopname.c_str(),
		 CURLFORM_END
		 );


	 curl_formadd(&formpost ,
		 &lastptr , 
		 CURLFORM_COPYNAME , "submit",
		 CURLFORM_END
		 );


	 curl_easy_setopt(curl, CURLOPT_URL, url.c_str());		
	 curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist); 
	 curl_easy_setopt(curl, CURLOPT_HTTPPOST , formpost);
	 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DistributeThread::write_data);
	 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
	 curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);                
	 curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, DistributeProgress::HandlerProgressFunc);
	 curl_easy_setopt(curl , CURLOPT_PROGRESSDATA,progress_data);
	 curl_easy_setopt(curl , CURLOPT_LOW_SPEED_LIMIT , CURL_LOW_SPEED_LIMIT/1024);
	 curl_easy_setopt(curl , CURLOPT_LOW_SPEED_TIME , CURL_LOW_SPEED_TIME);
	 curl_easy_setopt(curl , CURLOPT_CONNECTTIMEOUT_MS , 3000);

	 CURLcode code =	curl_easy_perform(curl);
	 if(code != CURLE_OK)
	 {
		 flag = false;
	 }
	 if(code == CURLE_OK)
	 {
		 long retcode;
		 curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
		 if(retcode != 200)
		 {
			 flag = false;
		 }
	 }

	 curl_easy_cleanup(curl);
	 curl_formfree(formpost);  
	 curl_slist_free_all (headerlist);  

	 return flag;
 }