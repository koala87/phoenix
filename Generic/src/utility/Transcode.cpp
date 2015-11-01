/**
 * Transcode related functions implementation
 * @author Shiwei Zhang
 * @date 2014.02.11
 */

#include <Windows.h>
#include <exception>
#include "utility/memman.h"
#include "utility/Transcode.h"
using namespace yiqiding::utility;

std::string yiqiding::utility::transcode(const std::string& s, CodePage::Charset from, CodePage::Charset to) {
	std::string result;

	int len = MultiByteToWideChar(from, 0, s.c_str(), -1, NULL, 0);
	if (len <= 0)
		return result;

	WCHAR* src = (WCHAR*)Lohas_MALLOC((len + 1) * sizeof(WCHAR));
	if (src == NULL)
		throw std::bad_alloc();
	MultiByteToWideChar(from, 0, s.c_str(), -1, src, len);
	src[len] = 0;
		
	len = WideCharToMultiByte(to, 0, src, -1, NULL, 0, NULL, NULL);
	char* dst = (char*)Lohas_MALLOC(len + 1);
	if (dst == NULL) {
		Lohas_FREE(src);
		throw std::bad_alloc();
	}
	WideCharToMultiByte(to, 0, src, -1, dst, len, NULL, NULL);
	dst[len] = 0;
	result = dst;
	Lohas_FREE(src);
	Lohas_FREE(dst);

	return result;
}

std::string yiqiding::utility::toMultiByte(const std::wstring &ws  , CodePage::Charset to)
{
	std::string result;
	int len;
	len = WideCharToMultiByte(to, 0, ws.c_str(), -1, NULL, 0, NULL, NULL);
	char* dst = (char*)Lohas_MALLOC(len + 1);
	if(dst == NULL){
		throw std::bad_alloc();
	}

	WideCharToMultiByte(to, 0,ws.c_str(), -1, dst, len, NULL, NULL);
	dst[len] = 0;
	result = dst;
	Lohas_FREE(dst);

	return result;
}

 std::wstring yiqiding::utility::toWideChar(const std::string& s,  CodePage::Charset from)
{
	std::wstring result;

	int len = MultiByteToWideChar(from, 0, s.c_str(), -1, NULL, 0);
	if (len <= 0)
		return result;

	WCHAR* src = (WCHAR*)Lohas_MALLOC((len + 1) * sizeof(WCHAR));
	if (src == NULL){
		throw std::bad_alloc();
	}
	MultiByteToWideChar(from, 0, s.c_str(), -1, src, len);
	src[len] = 0;
	result = src;
	Lohas_FREE(src);
	return result;
}