#include "utility/Logger.h"
#include "io/PCMAudio.h"
#include "utility/Utility.h"


using namespace yiqiding::ktv::audio;
using namespace yiqiding::utility;


void PCMAudioInput::waveInProc(HWAVEIN hwo, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2)
{
	PCMAudioInput *input = (PCMAudioInput *)dwInstance;
	
	if(uMsg == MM_WIM_DATA)
	{
		if(!input->getStatus())
			return;

		WAVEHDR *hdr = (PWAVEHDR)dwParam1;
		input->read(hdr->lpData , hdr->dwBytesRecorded);
	
		MMRESULT nRet = waveInAddBuffer(hwo , hdr , sizeof(WAVEHDR));
		if(nRet != MMSYSERR_NOERROR)
		{
			printf("waveInProc %d\n" , nRet);
		}
	}
	else if(uMsg == MM_WIM_CLOSE)
	{
		;
	}
	else if(uMsg == MM_WIM_OPEN)
	{
		;
	}
}

PCMAudioInput::PCMAudioInput():_havin(NULL)
{

}
PCMAudioInput::~PCMAudioInput(){
	
	stop();
}

bool PCMAudioInput::start(int nChannels ,int nSamplesPerSec ,int wBitsPerSample){

	_wfe.wFormatTag = WAVE_FORMAT_PCM;
	_wfe.nChannels = nChannels;
	_wfe.nSamplesPerSec = nSamplesPerSec;
	_wfe.nAvgBytesPerSec = nSamplesPerSec * nChannels * wBitsPerSample / 8;
	_wfe.nBlockAlign = nChannels * wBitsPerSample / 8;
	_wfe.wBitsPerSample = wBitsPerSample;
	_wfe.cbSize = 0;

	stop();

	MMRESULT nRet = waveInOpen (&_havin, WAVE_MAPPER, &_wfe, (DWORD_PTR) waveInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if(nRet != MMSYSERR_NOERROR)
		return false;

	int blocksize = (_wfe.nAvgBytesPerSec + pcm_frame - 1) / pcm_frame;

	int times =  1000 / pcm_frame;
	_len = (cache_time + times - 1) / times;

	_len = 2;

	_hdr = (WAVEHDR **)malloc(sizeof(WAVEHDR *) * _len);
	
	for(int i = 0 ; i < _len; ++i)
	{
		_hdr[i] = (WAVEHDR *)malloc(sizeof(WAVEHDR) + blocksize);
	//	yiqiding::utility::Logger::get("system")->log("malloc" + toString(_hdr[i]));
		memset(_hdr[i] , 0 , sizeof(WAVEHDR));
		_hdr[i]->dwLoops = 1;
		_hdr[i]->lpData = (char *)(_hdr[0] + 1); 
		_hdr[i]->dwBufferLength = blocksize;
	}

	for(int i = 0 ; i < _len ; ++i)
	{
		
		nRet = waveInPrepareHeader(_havin , _hdr[i] , sizeof(WAVEHDR));
		if(nRet != MMSYSERR_NOERROR)
			return false;
	}

	for(int i = 0 ; i < _len ; ++i)
	{
		nRet =  waveInAddBuffer (_havin, _hdr[i], sizeof (WAVEHDR)) ;
		if(nRet != MMSYSERR_NOERROR)
			return false;
	}


	nRet = waveInStart(_havin);
	if(nRet != MMSYSERR_NOERROR)
		return false;

	_status = true;
	return true;
}

bool PCMAudioInput::unprepare(){
	
	if(_hdr != NULL)
	{
		for( int i = 0 ; i < _len ; ++ i)
		{

			do{
				MMRESULT nRet = waveInUnprepareHeader(_havin , _hdr[i] , sizeof(WAVEHDR));
				if(nRet != MMSYSERR_NOERROR)
				{
					if(nRet == WAVERR_STILLPLAYING)
					{
						Sleep(100);		
						continue;
					}
					else
						return false;
				}
				break;
			}while(1);
			if(_hdr[i] != NULL)
			{
			//	yiqiding::utility::Logger::get("system")->log("free"  + toString(_hdr[i]) + +" threadid:"  + toString(GetCurrentThreadId()));
				free(_hdr[i]);
				_hdr[i] = NULL;
			}
		}
		free(_hdr);
		_hdr = NULL;
		_len = 0;
	}
	return true;
}

bool PCMAudioInput::stop()
{
	
	
	MMRESULT nRet;
	_status = false;
	
	if(_havin !=NULL )
	{		
		unprepare();

// 		if(waveInStop(_havin)!= MMSYSERR_NOERROR)
// 		{	
// 			return false;
// 		}

		nRet = waveInClose(_havin);
		if(nRet != MMSYSERR_NOERROR )
		{
			printf("PCMAudioInput stop\n");
			return false;
		}
		_havin = NULL;
	}
	return true;
}



PCMAudioOutPut::PCMAudioOutPut():_status(false),_hdr(NULL),_havout(NULL)
{
	
}

bool PCMAudioOutPut::unprepare(){
	MMRESULT nRet;
	if(_hdr != NULL)
	{
		for( int i = 0 ; i < _len ; ++ i)
		{
			do{
				nRet = waveOutUnprepareHeader(_havout , _hdr[i] , sizeof(WAVEHDR));
				if(nRet != MMSYSERR_NOERROR)
				{
					if(nRet == WAVERR_STILLPLAYING)
					{
						Sleep(100);
						continue;
					}
					else
					{
						printf("PCMAudioOutPut unprepare\n");
						return false;
					}
				}
				break;
			}while(1);
			
			if(_hdr[i] != NULL)
			{
				free(_hdr[i]);
				_hdr[i] = NULL;
			}
		}
		free(_hdr);
		_hdr = NULL;
		_len = 0;
	}
	return true;
}

bool PCMAudioOutPut::start(int nChannels ,int nSamplesPerSec ,int wBitsPerSample){
	
	_wfe.wFormatTag = WAVE_FORMAT_PCM;
	_wfe.nChannels = nChannels;
	_wfe.nSamplesPerSec = nSamplesPerSec;
	_wfe.nAvgBytesPerSec = nSamplesPerSec * nChannels * wBitsPerSample / 8;
	_wfe.nBlockAlign = nChannels * wBitsPerSample / 8;
	_wfe.wBitsPerSample = wBitsPerSample;
	_wfe.cbSize = 0;

	stop();
	
	MMRESULT nRet = waveOutOpen (&_havout, WAVE_MAPPER, &_wfe, (DWORD_PTR) PCMAudioOutPut::waveOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if(nRet != MMSYSERR_NOERROR)
		return false;

	_blocksize = (_wfe.nAvgBytesPerSec + pcm_frame - 1) / pcm_frame;

	int times =  1000 / pcm_frame;
	_len = (cache_time + times - 1) / times;

	_hdr = (WAVEHDR **)malloc(sizeof(WAVEHDR *) * _len);
	for(int i = 0 ; i < _len; ++i)
	{

		_hdr[i] = (WAVEHDR *)malloc(sizeof(WAVEHDR));
		memset(_hdr[i] , 0 , sizeof(WAVEHDR));
		_hdr[i]->dwLoops = 1;
		
	}

	for(int i = 0 ; i < _len ; ++i)
	{
		nRet = waveOutPrepareHeader(_havout , _hdr[i] , sizeof(WAVEHDR));
		if(nRet != MMSYSERR_NOERROR)
			return false;
	}

	_bstart = true;

	return true;
}

bool PCMAudioOutPut::stop(){

	MMRESULT nRet;


	_bstart = false;

	_status = false;
	
	unprepare();
	
	while(!_queueData.empty())
	{
		DataBuf data = _queueData.front();
		free(data._data);
		_queueData.pop();
	}
	
	if(_havout != NULL)
	{
		MMRESULT nRet = waveOutClose(_havout);
		if( nRet != MMSYSERR_NOERROR)
		{
			printf("stop waveOutClose\n");
			return false;
		}
			
		_havout = NULL;
	}

	return true;
	
}

void PCMAudioOutPut::write(const char *data , int len){
	
	yiqiding::MutexGuard guard(_mutex);

	DataBuf db(data , len);
	_queueData.push(db);
	
	if(!_status && _queueData.size() == _len)
	{
		for(int i = 0 ; i < _len ; ++i){
			db = _queueData.front();
			_queueData.pop();
			_hdr[i]->lpData = db._data;
			_hdr[i]->dwBufferLength = db._len;
			if(waveOutWrite(_havout , _hdr[i] , sizeof(WAVEHDR)) != MMSYSERR_NOERROR)
			{
				printf("write waveOutWrite \n");
				return;
			}
		}
		_status = true;
	}
}

void PCMAudioOutPut::writeQueue(WAVEHDR * hdr){
	yiqiding::MutexGuard guard(_mutex);
	
	if(hdr->lpData != NULL)
		free(hdr->lpData);

	if(_queueData.empty()){
		//printf("writeQueue empty\n");
		
		//quiety data
		hdr->lpData = (char *)malloc(hdr->dwBufferLength);
		memset(hdr->lpData , 0x88 ,  hdr->dwBufferLength);

		printf("write\n");

		if(waveOutWrite(_havout , hdr , sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
			printf("writeQueue waveOutWrite error\n");
		}
		
		return;
	}

	DataBuf db = _queueData.front();
	_queueData.pop();
	hdr->lpData = db._data;
	hdr->dwBufferLength = db._len;
	if(waveOutWrite(_havout , hdr , sizeof(WAVEHDR)) != MMSYSERR_NOERROR){
		printf("waveOutWrite error\n");
	}
}

void __stdcall PCMAudioOutPut::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,DWORD dwParam1, DWORD dwParam2){
	
	PCMAudioOutPut *out = (PCMAudioOutPut *)dwInstance;

	if(uMsg == MM_WOM_DONE)
	{		
		if(!out->getStatus())
			return;
		WAVEHDR *hdr = (PWAVEHDR)dwParam1;
		out->writeQueue(hdr);
	}
	else if(uMsg == MM_WOM_CLOSE)
	{
		;//out->unprepare();
	}
	else if(uMsg == MM_WOM_OPEN)
	{
		;
	}
}