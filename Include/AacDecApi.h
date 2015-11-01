#include "faad.h"


typedef struct AAC_DECODEINFO
{//aac解码重要信息，其中前三项可以外部配置，其余的从aac文件中获取
	unsigned char iAACType;				// AAC object type 
	unsigned char iBitsPerSample;		//
	unsigned long iSampFreq;			// frequency 

	unsigned char iMpegVersion;			// 1:mpeg 2 , 0:mpeg 4
	unsigned char iChannel;				// channel number
	unsigned char iHeaderType;			// 0 = Raw; 1 = ADIF; 2 = ADTS
	unsigned long iDataRate;			// Datarate 

}*LPAAC_DECODEINFO;


class AacDecApi{

public: 
	AacDecApi(void);
	~AacDecApi(void);

	bool Initialize();
	bool StartDecode(unsigned char* pBuffer,int nBufLen);
	//bool GetOutputBufferFormat(void &wfx);
	bool DecodeBuffer(unsigned char*pBufferIn,int nBufLenIn,unsigned char **pBufferOut,int *nBufLenOut,int *nBufInComsuned);
	bool Uninitialize();

	bool GetDecodeInfo(LPAAC_DECODEINFO &pInfo);
private:
	NeAACDecHandle		m_hFaadDec;
	LPAAC_DECODEINFO	m_lpInfo;
	unsigned char*		m_pInLeftBuffer;
	unsigned char*		m_pOutBuffer;
	int					m_nOutBufferLen;
	long				m_lStreamSize;
	long				m_lSampleRate;


};