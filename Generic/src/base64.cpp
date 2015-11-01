#include "base64.h"
#include "utility/memman.h"


#include <string>  



BYTE Decode_GetByte(char c);
char Encode_GetChar(BYTE num);

//===================================
//    Base64 解码
//===================================
BYTE Decode_GetByte(char c)
{
	if(c == '+')
		return 62;
	else if(c == '/')
		return 63;
	else if(c <= '9')
		return (BYTE)(c - '0' + 52);
	else if(c == '=')
		return 64;
	else if(c <= 'Z')
		return (BYTE)(c - 'A');
	else if(c <= 'z')
		return (BYTE)(c - 'a' + 26);
	return 64;
}

//解码
size_t Base64_Decode(char *pDest, const char *pSrc, size_t srclen)
{
	BYTE input[4];
	size_t i, index = 0;
	for(i = 0; i < srclen; i += 4)
	{
		//byte[0]
		input[0] = Decode_GetByte(pSrc[i]);
		input[1] = Decode_GetByte(pSrc[i + 1]);
		pDest[index++] = (input[0] << 2) + (input[1] >> 4);

		//byte[1]
		if(pSrc[i + 2] != '=')
		{
			input[2] = Decode_GetByte(pSrc[i + 2]);
			pDest[index++] = ((input[1] & 0x0f) << 4) + (input[2] >> 2);
		}

		//byte[2]
		if(pSrc[i + 3] != '=')
		{
			input[3] = Decode_GetByte(pSrc[i + 3]);
			pDest[index++] = ((input[2] & 0x03) << 6) + (input[3]);
		}            
	}

	//null-terminator
	pDest[index] = 0;
	return index;
}

size_t Base64_Decode(char *pDest , const std::string &src)
{
	return Base64_Decode(pDest , src.c_str() , src.length());
}
//===================================
//    Base64 编码
//===================================
char Encode_GetChar(BYTE num)
{
	return 
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"+/="[num];
}

//编码
size_t Base64_Encode(char *pDest, const char *pSrc, size_t srclen)
{
	BYTE input[3], output[4];
	size_t i, index_src = 0, index_dest = 0;
	for(i = 0; i < srclen; i += 3)
	{
		//char [0]
		input[0] = pSrc[index_src++];
		output[0] = (BYTE)(input[0] >> 2);
		pDest[index_dest++] = Encode_GetChar(output[0]);

		//char [1]
		if(index_src < srclen)
		{
			input[1] = pSrc[index_src++];
			output[1] = (BYTE)(((input[0] & 0x03) << 4) + (input[1] >> 4));
			pDest[index_dest++] = Encode_GetChar(output[1]);
		}
		else
		{
			output[1] = (BYTE)((input[0] & 0x03) << 4);
			pDest[index_dest++] = Encode_GetChar(output[1]);
			pDest[index_dest++] = '=';
			pDest[index_dest++] = '=';
			break;
		}

		//char [2]
		if(index_src < srclen)
		{
			input[2] = pSrc[index_src++];
			output[2] = (BYTE)(((input[1] & 0x0f) << 2) + (input[2] >> 6));
			pDest[index_dest++] = Encode_GetChar(output[2]);
		}
		else
		{
			output[2] = (BYTE)((input[1] & 0x0f) << 2);
			pDest[index_dest++] = Encode_GetChar(output[2]);
			pDest[index_dest++] = '=';
			break;
		}

		//char [3]
		output[3] = (BYTE)(input[2] & 0x3f);
		pDest[index_dest++] = Encode_GetChar(output[3]);
	}
	//null-terminator
	pDest[index_dest] = 0;
	return index_dest;
}

size_t Base64_Encode(char *pDest , const std::string &src)
{
	return Base64_Encode(pDest , src.c_str() , src.length());
}
//百分号编码
//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

int UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, int bUpperCase)
{
	if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return 0;

	size_t len_ascii = strlen(szSrc);
	if(len_ascii == 0)
	{
		pBuf[0] = 0;
		return 1;
	}

	//先转换到UTF-8
	char baseChar = bUpperCase ? 'A' : 'a';
	int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)Lohas_MALLOC((cchWideChar + 1) * sizeof(WCHAR));
	if(pUnicode == NULL)
		return 0;
	MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
	LPSTR pUTF8 = (LPSTR)Lohas_MALLOC((cbUTF8 + 1) * sizeof(CHAR));
	if(pUTF8 == NULL)
	{
		Lohas_FREE(pUnicode);
		return 0;
	}
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
	pUTF8[cbUTF8] = '\0';

	unsigned char c;
	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)pUTF8;
	unsigned char *pDest = (unsigned char*)pBuf;
	while(*pSrc && cbDest < cbBufLen - 1)
	{
		c = *pSrc;
		if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
		{
			*pDest = c;
			++pDest;
			++cbDest;
		}
		else if(c == ' ')
		{
			*pDest = '+';
			++pDest;
			++cbDest;
		}
		else
		{
			//检查缓冲区大小是否够用？
			if(cbDest + 3 > cbBufLen - 1)
				break;
			pDest[0] = '%';
			pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
			pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
			pDest += 3;
			cbDest += 3;
		}
		++pSrc;
	}
	//null-terminator
	*pDest = '\0';
	Lohas_FREE(pUnicode);
	Lohas_FREE(pUTF8);
	return 1;
}

//解码后是utf-8编码
int UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
{
	if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
		return 0;

	size_t len_ascii = strlen(szSrc);
	if(len_ascii == 0)
	{
		pBuf[0] = 0;
		return 1;
	}

	char *pUTF8 = (char*)new char[len_ascii + 1];
	if(pUTF8 == NULL)
		return 0;

	int cbDest = 0; //累加
	unsigned char *pSrc = (unsigned char*)szSrc;
	unsigned char *pDest = (unsigned char*)pUTF8;
	while(*pSrc)
	{
		if(*pSrc == '%')
		{
			*pDest = 0;
			//高位
			if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
				*pDest += (pSrc[1] - 'A' + 10) * 0x10;
			else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
				*pDest += (pSrc[1] - 'a' + 10) * 0x10;
			else
				*pDest += (pSrc[1] - '0') * 0x10;

			//低位
			if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
				*pDest += (pSrc[2] - 'A' + 10);
			else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
				*pDest += (pSrc[2] - 'a' + 10);
			else
				*pDest += (pSrc[2] - '0');

			pSrc += 3;
		}
		else if(*pSrc == '+')
		{
			*pDest = ' ';
			++pSrc;
		}
		else
		{
			*pDest = *pSrc;
			++pSrc;
		}
		++pDest;
		++cbDest;
	}
	//null-terminator
	*pDest = '\0';
	++cbDest;

	int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
	LPWSTR pUnicode = (LPWSTR)Lohas_MALLOC(cchWideChar * sizeof(WCHAR));
	if(pUnicode == NULL)
	{
		Lohas_FREE(pUTF8);
		return FALSE;
	}
	MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
	WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
	Lohas_FREE(pUTF8);
	Lohas_FREE(pUnicode);
	return 1;
}