/**
 * Romaji related functions implementation
 * @author Shiwei Zhang , Yuchun Zhang
 * @date 2014.03.14
 */

#include <WinSock2.h>
#include <Windows.h>
#include <cctype>
#include "kakasi.h"
#include "utility/memman.h"
#include "utility/Romaji.h"
#include "Exception.h"
using namespace yiqiding::utility;

#pragma comment(lib, "kakasi")

static bool kakasi_initialised = false;

inline bool isalpha(wchar_t c) {
	return (L'a' <= c && c <= L'z') || (L'A' <= c && c <= L'Z');
}
inline bool isvowel(char c) {
	return c == 'a' || c == 'i' || c == 'u' || c == 'e' || c == 'o'; 
}
std::string convert(WCHAR* s, size_t len);

std::string yiqiding::utility::toRomaji(const std::string& s, char delimiter, bool lower_letter, CodePage::Charset encode) {
	if (!kakasi_initialised) {
		const char* argv[] = {"kakasi", "-Ea", "-Ha", "-Ka", "-Ja", "-s"};
		if (kakasi_getopt_argv(sizeof(argv)/sizeof(char*), (char**)argv) != 0)
			throw yiqiding::Exception("kakasi", "Fail to initialise", __FILE__, __LINE__);
		kakasi_initialised = true;
	}

	int len = MultiByteToWideChar(encode, 0, s.c_str(), -1, NULL, 0);
	if (len <= 0)
		return "";
	WCHAR* src = (WCHAR*)new char[len * 2];
	if (src == NULL)
		throw std::bad_alloc();
	MultiByteToWideChar(encode, 0, s.c_str(), -1, src, len);

	// Phase 1: Convert Japanese to Romaji
	std::string target;
	WCHAR* begin = src;
	WCHAR* end = src + len;
	WCHAR* p = begin;
	bool last_done = true;
	while(p != end) {
		if (isalpha(*p)) {
			if (!last_done) {
				target.push_back(' ');
				last_done = true;
				target.append(convert(begin, p - begin));
				target.push_back(' ');
			}
			target.push_back((char)*p);
			begin = p;
			begin++;
		} else
			last_done = false;
		++p;
	}
	if (!last_done) {
		target.push_back(' ');
		target.append(convert(begin, p - begin));
	}
	Lohas_FREE(src);

	// Phase 2: Format & Output
	std::string result;
	last_done = true;
	len = (int)target.size();

	for (int i = 0; i < len; ++i)
		if (std::isalpha(target[i])) {
			if (!last_done) {
				if (!result.empty())
					result.push_back(delimiter);
				last_done = true;
			}
			if (lower_letter)
				result.push_back(tolower(target[i]));
			else
				result.push_back(target[i]);
		} else
			last_done = false;

	return result;
}

std::string convert(WCHAR* s, size_t len) {
	int dst_len = WideCharToMultiByte(CodePage::SJIS, 0, s, (int)len, NULL, 0, NULL, NULL);
	char* dst = (char*)new char[dst_len+1];
	if (dst == NULL)
		return "";
	WideCharToMultiByte(CodePage::SJIS, 0, s, (int)len, dst, dst_len, NULL, NULL);
	dst[dst_len] = 0;
	char* romaji = kakasi_do(dst);
	Lohas_FREE(dst);
	std::string target = romaji;
	kakasi_free(romaji);

	std::string result;
	len = target.size();
	for (size_t i = 0; i < len; ++i) {
		if (std::isalpha(target[i])) {
			if (i + 1 < len && target[i] == target[i+1])
				continue;
			result.push_back(target[i]);
			if (!isvowel(target[i]) && i + 1 < len) {
				result.push_back(target[++i]);
				if ((target[i] == 's' || target[i] == 'h' || target[i] == 'y') && i + 1 < len)
					result.push_back(target[++i]);
			}
		}
		result.push_back(' ');
	}

	return result;
}
