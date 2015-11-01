/**
 * Transcode related functions
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#pragma once

#include <string>
#include <Windows.h>
 
namespace yiqiding { namespace utility {
	class CodePage {
	public:
		enum Charset {
			UTF8	=	65001,
			GBK		=	936,
			SJIS	=	932
		};
	};

	// Char set conversion , UTF8 TO GBK 
	std::string transcode(const std::string& s, CodePage::Charset from, CodePage::Charset to);
	
	// from unicode to UTF8,GBK ...
	std::string toMultiByte(const std::wstring &s,  CodePage::Charset to);

	// from GBK,UTF8 to unicode , must remember free return value.
	 std::wstring toWideChar(const std::string &s , CodePage::Charset from = CodePage::UTF8);
}}
