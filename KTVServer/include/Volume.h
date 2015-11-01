/*
@author Zhangyuchun
@Date	2014-12-24
@Content //������������ʱ�� ���԰�adurl firewarn messagerule volume ����������������úϲ���
*/

#pragma once
#include <string>
#include "Thread.h"

namespace yiqiding { namespace ktv{ 


	const std::string VlPath = "volume.json";

	class Volume
	{
	public:
		void load(const std::string &path);
		bool save(const std::string &path , const std::string &volumeInfo);
		std::string getVolumeInfo() const ;
	private:
		std::string _volumeInfo;
		mutable yiqiding::Mutex _mutex;
	};

} }