// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the AUDIOMIXING_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// AUDIOMIXING_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef AUDIOMIXING_EXPORTS
#define AUDIOMIXING_API __declspec(dllexport)
#else
#define AUDIOMIXING_API __declspec(dllimport)
#endif

EXTERN_C AUDIOMIXING_API int _stdcall AudioMixing(const char ppSrc[][4096], const int nDataLen, const int nAudioNum, char *pDest);


EXTERN_C AUDIOMIXING_API int _stdcall InitAudioMixing();

EXTERN_C AUDIOMIXING_API int _stdcall FreeAudioMixing();