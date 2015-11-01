/**
 * Distribute Files
 * Lohas Network Technology Co., Ltd
 * @author Yuchun zhang
 * @date 2014.03.10
 */

#pragma once
#include <vector>
#include <string>

namespace yiqiding { namespace io{

	class FileDistribute
	{
	private:
		std::vector<std::string> _vecPath;
	public:
		virtual ~FileDistribute(){}


		//// File
		
		//向所有路径分发文件，全部分发成功，返回true；任何一个路径分发失败，返回false.
		//ifexistfail为true，表示如果文件存在返回失败；否则进行方式覆盖分发。
		bool addFile(const std::string &pathname ,const std::string &filename , bool ifexistfail) const;

		//向所有路径尝试删除文件，不保证删除成功。
		void delFile(const std::string &filename) const;	

		//所有路径中至少某个路径存在该文件，则返回true,否则所有路径都不村在该文件，返回false.
		bool isExistInOnePath(const std::string &filename) const;

		//所有路径都存在该文件，则返回true，否则返回false.
		bool isExistInAllPath(const std::string &filename) const;
		

		//// Path
		bool isExistPath(const std::string &path) const;
		bool addPath(const std::string &path);
		
	};

}}

