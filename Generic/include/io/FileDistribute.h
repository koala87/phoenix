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
		
		//������·���ַ��ļ���ȫ���ַ��ɹ�������true���κ�һ��·���ַ�ʧ�ܣ�����false.
		//ifexistfailΪtrue����ʾ����ļ����ڷ���ʧ�ܣ�������з�ʽ���Ƿַ���
		bool addFile(const std::string &pathname ,const std::string &filename , bool ifexistfail) const;

		//������·������ɾ���ļ�������֤ɾ���ɹ���
		void delFile(const std::string &filename) const;	

		//����·��������ĳ��·�����ڸ��ļ����򷵻�true,��������·���������ڸ��ļ�������false.
		bool isExistInOnePath(const std::string &filename) const;

		//����·�������ڸ��ļ����򷵻�true�����򷵻�false.
		bool isExistInAllPath(const std::string &filename) const;
		

		//// Path
		bool isExistPath(const std::string &path) const;
		bool addPath(const std::string &path);
		
	};

}}

