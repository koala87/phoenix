#include "yiqiding/ktv/KTVCloud.h"
#include "yiqiding/net/FileCurlDistribute.h"
using namespace yiqiding::ktv;

int main()
{

	/* std::string str = "{\"test\":\"\u6ca1\u6709\u4e0a\u4f20\u5217\u8868\u7c7b\u578b\"}";
	Json::Reader reader;
	Json::Value root;
	if(!reader.parse(str , root))
		return 0;
	str = root["test"].asString();

	printf("%s\n",str.c_str());*/

	cloud::MusicCloud *pcloud = new cloud::MusicCloud();
//	yiqiding::net::DistributeContent::getInstance()->addHost(new yiqiding::net::DistributeCurl("http://192.168.1.199/upload.php"));
//	pcloud->unzip();
//	pcloud->execute();
//	pcloud->upload();
	pcloud->load(NULL);
	pcloud->check();
//	pcloud->download();
}