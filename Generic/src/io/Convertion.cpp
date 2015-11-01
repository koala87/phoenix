// Convertion.cpp : Defines the exported functions for the DLL application.
//

#include <stdlib.h>
#include <stdio.h>
#include <windows.h> 
#include <tchar.h>
#include <strsafe.h>
#include "Convertion.h"

extern "C"
{
	typedef enum _Pipi_Info
	{
		FFMPEG_GETVOLUME_MAP0=0,
		FFMPEG_GETVOLUME_MAP1,
		MEDIAINFO_GETATTR,
		MAX,
	}Pipi_Info;

	typedef struct _Volume_Type_
	{
		int track;
		double volume;
	}Volume_Type;

	typedef struct _Media_Info_
	{
		int width;
		int height;
		char scantype[64];
	}Media_Info;

	Media_Info sMediaInfo = {0};
	Volume_Type sVolumeType = {0};

#define BUFSIZE 4096 

#define RETURN_NOERR 0
#define RETURN_STSTEMCMDERROR -10
#define RETURN_NOCOMDLINE -9
#define RETURN_HANDLEINFORERROR -8
#define RETURN_CREATEPIPEERROR -7
#define RETURN_CREATEPROCESSERROR -6
#define RETURN_NOVEDIOFILE -5
#define RETURN_NOTRACKFILE -4
#define RETURN_NODESTFILE -3
#define RETURN_NOSRCFILE -2
#define RETURN_ERR -1

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
	HANDLE g_hChildStd_IN_Rd = NULL;

	int CreateChildProcess(const char *pCmdLine); 
	char* ReadFromPipe(Pipi_Info Pipi);
	void ErrorExit(PTSTR); 

	int splitstr(const char *pSrc, const char *pFirst, const char *pSecond, char *pSplit)
	{
		char *p1 = NULL, *p2 = NULL;
		int nLen = 0;
		char *szpSrc = NULL;
		if(!pSrc || !pFirst || !pSecond)
			return 0;

		nLen = strlen(pSrc);
		szpSrc = (char *)malloc(nLen+1);

		memset(szpSrc, 0x00, nLen+1);
		memcpy(szpSrc, pSrc, nLen);

		p1 = strstr(szpSrc, pFirst);
		if(p1 != NULL)
		{
			p1 = strstr(p1, ": ");
			if(p1 != NULL)
			{
				int nLen = strlen(": ");
				p2 = strstr(p1, pSecond);
				if(p2 != NULL)
				{	
					memcpy(pSplit, p1+nLen, p2-p1-nLen);
					return strlen(pSplit);
				}
			}
		}
		return 0;
	}

	int CheckMediaInfo(const char *pFile)
	{
		char szFile[32760] = {0};
		char szCmdLine[4096] = {0};
		FILE * fp;
		char buffer[4096] = {0};
		int nRet = 0;
		char szAttr[32] = {0};
		int nLen = 0;

		if( !pFile || (pFile && strlen(pFile) == 0))
			return RETURN_NOSRCFILE;

		memcpy(szFile, pFile, strlen(pFile));
		memset(&sMediaInfo, 0x00, sizeof(sMediaInfo));
		sprintf(szCmdLine,"MediaInfo.exe %s",szFile);

		fp = _popen(szCmdLine, "r");

		while(!feof(fp)){
			memset(buffer, 0x00, sizeof(buffer));
			fgets(buffer, sizeof(buffer), fp);

			if(strlen(buffer) == 0)
				break;

			if(sMediaInfo.width == 0)
			{
				memset(szAttr, 0x00, sizeof(szAttr));
				nLen = splitstr(buffer, "Width", " pixels", szAttr);
				if(nLen > 0)
				{
					sMediaInfo.width = atoi(szAttr);
				}
			}
			if(sMediaInfo.height == 0)
			{
				memset(szAttr, 0x00, sizeof(szAttr));
				nLen = splitstr(buffer, "Height", " pixels", szAttr);
				if(nLen > 0)
				{
					sMediaInfo.height = atoi(szAttr);
				}
			}
			if(strlen(sMediaInfo.scantype) == 0)
			{
				memset(szAttr, 0x00, sizeof(szAttr));
				nLen = splitstr(buffer, "Scan type", "\n", szAttr);
				if(nLen > 0)
				{
					memcpy(sMediaInfo.scantype, szAttr, nLen);
					break;
				}
			}
		}
		_pclose(fp);

		printf("\n sMediaInfo.scantype = %s \n sMediaInfo.height = %d \n sMediaInfo.width = %d \n",
			sMediaInfo.scantype, sMediaInfo.height, sMediaInfo.width);

		return RETURN_NOERR;
	}

	int CheckVolume(const char *pFile)
	{
		char *pVolumeMap0, *pVolumeMap1;
		char szFile[32760] = {0};
		char szCmdLine[4096] = {0};
		SECURITY_ATTRIBUTES saAttr; 
		int nRet = 0;

		if( !pFile || (pFile && strlen(pFile) == 0))
			return RETURN_NOSRCFILE;

		memcpy(szFile, pFile, strlen(pFile));

		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
		saAttr.bInheritHandle = TRUE; 
		saAttr.lpSecurityDescriptor = NULL; 

		if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
			return RETURN_CREATEPIPEERROR; 

		if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
			return RETURN_HANDLEINFORERROR; 

		sprintf(szCmdLine,"ffmpeg.exe -i %s -map 0:a:0 -af volumedetect -f null /dev/NUL",szFile);

		nRet = CreateChildProcess(szCmdLine);
		if(nRet != RETURN_NOERR)
			return nRet;

		pVolumeMap0 = ReadFromPipe(FFMPEG_GETVOLUME_MAP0); 

		memset(&sVolumeType, 0x00, sizeof(sVolumeType));
		if(pVolumeMap0 && strlen(pVolumeMap0) > 0)
		{
			char szVolume[16] = {0}; 
			double dVolume = 0.0;
			memcpy(szVolume, pVolumeMap0, strlen(pVolumeMap0));
			sVolumeType.track = 1;
			dVolume = atof(szVolume);
			sVolumeType.volume = -13.0 - dVolume;
		}
		else
		{
			return RETURN_NOTRACKFILE;
		}

		if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
			return RETURN_CREATEPIPEERROR; 

		if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
			return RETURN_HANDLEINFORERROR; 

		sprintf(szCmdLine,"ffmpeg.exe -i %s -map 0:a:1 -af volumedetect -f null /dev/NUL",szFile);

		nRet = CreateChildProcess(szCmdLine);

		if(nRet != RETURN_NOERR)
			return nRet;

		pVolumeMap1 = ReadFromPipe(FFMPEG_GETVOLUME_MAP1);

		if(pVolumeMap1 && strlen(pVolumeMap1) > 0)
		{
			sVolumeType.track = 2;
		}
		printf("sVolumeType.track = %d\n\n", sVolumeType.track);
		
		return RETURN_NOERR;
	}

	int CreateChildProcess(const char *pCmdLine)	
	{ 
		char szCmdline[4098] = {0};
		if( !pCmdLine )
			return RETURN_NOCOMDLINE;

		memcpy(szCmdline, pCmdLine, strlen(pCmdLine));

		PROCESS_INFORMATION piProcInfo; 
		STARTUPINFOA siStartInfo;
		BOOL bSuccess = FALSE; 

		ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
		ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
		siStartInfo.cb = sizeof(STARTUPINFO); 
		siStartInfo.hStdError = g_hChildStd_OUT_Wr;
		siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
		siStartInfo.hStdInput = g_hChildStd_IN_Rd;
		siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
		siStartInfo.wShowWindow = FALSE;

		bSuccess = CreateProcessA(NULL, 
			szCmdline,     // command line 
			NULL,          // process security attributes 
			NULL,          // primary thread security attributes 
			TRUE,          // handles are inherited 
			0,             // creation flags 
			NULL,          // use parent's environment 
			NULL,          // use parent's current directory 
			&siStartInfo,  // STARTUPINFO pointer 
			&piProcInfo);  // receives PROCESS_INFORMATION 

		printf("\n CreateProcessA bSuccess = %d \n", bSuccess);

		if ( ! bSuccess ) 
		{
			return RETURN_CREATEPROCESSERROR;
		}
		else 
		{
			CloseHandle(piProcInfo.hProcess);
			CloseHandle(piProcInfo.hThread);
		}

		return RETURN_NOERR;
	}

	char* ReadFromPipe(Pipi_Info Pipi)
	{ 
		DWORD dwRead; 
		CHAR chBuf[BUFSIZE+1] = {0}; 
		char szchBuf[BUFSIZE+1] = {0};
		int nLen = 0;
		char szVolume[32] = {0};
		BOOL bSuccess = FALSE;
		BOOL bContinue = FALSE;
		char *p1 = NULL, *p2 = NULL;

		for (;;) 
		{ 
			memset(chBuf, 0x00, BUFSIZE+1);
			bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
			printf("\n chBuf = %s \n, dwRead = %d \n", chBuf, dwRead);
			if(bContinue)
			{
				memcpy(&szchBuf[nLen], chBuf, dwRead);
			}

			if( ! bSuccess || dwRead == 0 ) 
			{
				CloseHandle(g_hChildStd_OUT_Rd);
				break; 
			}

			if(Pipi == FFMPEG_GETVOLUME_MAP1)
			{
				p1 = strstr(chBuf, "Stream map '0:a:1' matches no streams");
				if(p1 != NULL)
				{
					CloseHandle(g_hChildStd_OUT_Rd);
					return NULL;
				}
			}


			if(Pipi == FFMPEG_GETVOLUME_MAP0)
			{
				p1 = strstr(chBuf, "Stream map '0:a:0' matches no streams");
				if(p1 != NULL)
				{
					CloseHandle(g_hChildStd_OUT_Rd);
					return NULL;
				}
			}

			if(!bContinue)
			{
				p1 = strstr(chBuf, "mean_volume: ");
			}
			else
			{
				p1 = strstr(szchBuf, "mean_volume: ");
			}
			printf("\np1 = %s\n", p1);
			if(p1 != NULL)
			{
				int nLen = strlen("mean_volume: ");
				p2 = strstr(p1, " dB");
				printf("\np2 = %s\n", p2);
				if(p2 != NULL)
				{	
					memcpy(szVolume, p1+nLen, p2-p1-nLen);
					printf("\n szVolume = %s\n", szVolume);
					bContinue = FALSE;
					CloseHandle(g_hChildStd_OUT_Rd);
					return szVolume;
				}
				else
				{
					memcpy(szchBuf, chBuf, dwRead);
					bContinue = TRUE;
					continue;
				}
			}
		}
		CloseHandle(g_hChildStd_OUT_Rd);
		return NULL;
	}

	void ErrorExit(PTSTR lpszFunction) 
	{ 
		ExitProcess(1);
	}

	 int _stdcall Convertion(const char *pSrc, const char *pDest)
	{
		char SrcPath[32760] = {0};
		char cmdline[32760] = {0};
		char DestPath[32760] = {0};
		bool bScantypeInterlaced = FALSE;
		int nMediaInfoRet = RETURN_ERR;
		int nSystemRet = RETURN_ERR;
		double volume = 13.0;
		if(!pSrc || (pSrc && strlen(pSrc) == 0))
		{
			return RETURN_NOSRCFILE;
		}
		if(!pDest || (pDest && strlen(pDest) == 0))
		{
			return RETURN_NODESTFILE;
		}
		int nSrcLen = strlen(pSrc);
		int nDestLen = strlen(pDest);
		memcpy(SrcPath, pSrc, nSrcLen);
		memcpy(DestPath, pDest, nDestLen);

		nMediaInfoRet = CheckMediaInfo(SrcPath);
		if(nMediaInfoRet == RETURN_NOERR)
		{
			if(sMediaInfo.height == 0 
				|| sMediaInfo.width == 0)
			{
				return RETURN_NOVEDIOFILE;
			}
			if(strlen(&sMediaInfo.scantype[0]) > 0
				&& memcmp(&sMediaInfo.scantype, "Interlaced", strlen("Interlaced")) == 0)
			{
				bScantypeInterlaced = TRUE;
			}
			else
			{
				bScantypeInterlaced = FALSE;
			}
		}
		else
		{
			return nMediaInfoRet;
		}
		nMediaInfoRet = CheckVolume(SrcPath);
		if(nMediaInfoRet != RETURN_NOERR)
			return nMediaInfoRet;

		if(sVolumeType.track == 0)
			return RETURN_NOTRACKFILE;

		volume = sVolumeType.volume;

		if(sVolumeType.track == 2)
		{
			if(bScantypeInterlaced)
			{
				sprintf(cmdline,"ffmpeg.exe -y -i %s -map 0:v:0 -c:v h264 -qscale 0 -force_key_frames expr:gte(t,n_forced*2) -deinterlace -map 0:a:0 -c:a:0 aac -strict -2 -qscale 0 -map 0:a:1 -c:a:1 aac -strict -2 -qscale 0 -af volume=%0.2lfdB -movflags faststart %s",SrcPath, volume, DestPath);
			}
			else
			{
				sprintf(cmdline,"ffmpeg.exe -y -i %s -map 0:v:0 -c:v h264 -qscale 0 -force_key_frames expr:gte(t,n_forced*2) -map 0:a:0 -c:a:0 aac -strict -2 -qscale 0 -map 0:a:1 -c:a:1 aac -strict -2 -qscale 0 -af volume=%0.2lfdB -movflags faststart %s",SrcPath, volume, DestPath);
			}
		}
		else if(sVolumeType.track == 1)
		{
			if(bScantypeInterlaced)
			{
				sprintf(cmdline,"ffmpeg.exe -y -i %s -map 0:v:0 -c:v h264 -qscale 0 -force_key_frames expr:gte(t,n_forced*2) -deinterlace -map 0:a:0 -c:a:0 aac -strict -2 -qscale 0 -af volume=%0.2lfdB -movflags faststart %s",SrcPath, volume, DestPath);
			}
			else
			{
				sprintf(cmdline,"ffmpeg.exe -y -i %s -map 0:v:0 -c:v h264 -qscale 0 -force_key_frames expr:gte(t,n_forced*2) -map 0:a:0 -c:a:0 aac -strict -2 -qscale 0 -af volume=%0.2lfdB -movflags faststart %s",SrcPath, volume, DestPath);
			}
		}
		else
		{
			return RETURN_NOTRACKFILE;
		}

		nSystemRet = system(cmdline);

		if(nSystemRet == 0)
		{
			return RETURN_NOERR;
		}
		else
		{
			return RETURN_STSTEMCMDERROR;
		}

		return RETURN_NOERR;
	}
}