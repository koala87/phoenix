#pragma once
#include <WinSock2.h>
#include <Windows.h>
#include <string>

size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen);
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen); 
size_t Base64_Encode(char *pDest , const std::string &src);
size_t Base64_Decode(char *pDest , const std::string &src);

int UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase);
int UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);