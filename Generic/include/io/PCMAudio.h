/*
播放及录制pcm流

*/

#pragma once
#include "windows.h"
#include <mmsystem.h>
#include <queue>
#include "Thread.h"
namespace yiqiding { namespace ktv{ namespace audio{

static const int pcm_frame = 50;//25;

static const int cache_time = 50;//200;
//
//300ms的读写缓存
//

class PCMAudioInput{
private:
	HWAVEIN		 _havin;
	WAVEHDR		**_hdr;
	int			_len;
	WAVEFORMATEX _wfe;
	bool		 _status;
	static void WINAPI waveInProc(HWAVEIN hwo, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2);
	virtual void read(void *data , int len){

	}
	bool unprepare();
public:

	bool start(int nChannels ,int nSamplesPerSec ,int wBitsPerSample);
	bool stop();
	bool getStatus() { return _status;}
	PCMAudioInput();
	~PCMAudioInput();
};


class PCMAudioOutPut{
public:
	struct DataBuf{
		char *	_data;
		int		_len;
		
		DataBuf(const char *data , int len){
			_data = (char *)malloc(len);
			_len = len;
			memcpy(_data , data ,len);
		}
	
	};
private:
	HWAVEOUT	_havout;
	WAVEFORMATEX _wfe;
	bool	 _status;
	yiqiding::Mutex	_mutex;

	WAVEHDR		**_hdr;
	int			_len;

	bool	_bstart;

	int			_blocksize;

	std::queue<DataBuf>	_queueData;


	static void WINAPI waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2);
	void writeQueue(WAVEHDR * hdr);
	bool unprepare();

public:
	bool start(int nChannels ,int nSamplesPerSec ,int wBitsPerSample);
	bool stop();
	void write(const char *data , int len);
	bool getStatus() { return _bstart;}
	~PCMAudioOutPut(){stop();}
	PCMAudioOutPut();
};


}}}