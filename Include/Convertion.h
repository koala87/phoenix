// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CONVERTION_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CONVERTION_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CONVERTION_EXPORTS
#define CONVERTION_API __declspec(dllexport)
#else
#define CONVERTION_API __declspec(dllimport)
#endif

extern "C" CONVERTION_API int _stdcall Convertion(const char *pSrc, const char *pDest);