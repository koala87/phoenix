/**
	内存管理	
	MARCO USE_MEM_POOL 强制使用new.
	说明:
	原想重载全局，但由于：
	重载 operator new(size_t size), void operator delete(void*) , void operator delete[](void*) . 
	这样所有被包含在主程序中的new操作符都会被替换，但二进制版本的会连接旧的方法，所以有可能导致旧的new，使用新的delete，或新的new，使用旧的delete。
	这钟情况一般头文件中有new，delete操作。

	所以，只抽象出C的内存接口。
**/
#pragma once

//c alloc
void * smalloc(size_t size , const char *file , size_t no);
void sfree(void *mem);
void *scalloc(size_t n , size_t size , const char *file , size_t no);
void *srealloc(void *mem_address , size_t size , const char *file , size_t no);

//
namespace yiqiding{namespace net{
	class DynamicData;
}}
//back json
void showAvailable(yiqiding::net::DynamicData &data);
void showUsed(yiqiding::net::DynamicData &data);



//#define DEBUG_NEW					new(__FILE__ , __LINE__)
#define Lohas_MALLOC(x)				smalloc(x , __FILE__ , __LINE__)
#define Lohas_FREE(x)				sfree(x)
#define Lohas_CALLOC(x , y)			scalloc(x , y , __FILE__ , __LINE__)
#define Lohas_REALLOC(x , y)		srealloc(x , y , __FILE__ , __LINE__)



//operator new
#ifdef USE_MEM_POOL
void* operator new(size_t size, const char *file , size_t no);
#endif

//operator delete
//void operator delete(void*) ;          
//void operator delete[](void*); 


//void* operator new(size_t, int , const char *file , size_t no);
//void* operator new[] (size_t , const char *file , size_t no);     
//void* operator new[] (size_t, int , const char * file , size_t no); 

//void operator delete(void*, size_t);  
//void operator delete[](void*, size_t); 

//void* operator new(size_t size);
//void* operator new(size_t , int);
//void* operator new[] (size_t );     
//void* operator new[] (size_t, int); 

//#define DEBUG_NEW(x , y)			new(x , y , __FILE__ , __LINE__)
//#define DEBUG_NEW[](x)				new[](x , __FILE__ , __LINE__)
//#define DEBUG_NEW[](x , y)			new[](x , y , __FILE__ , __LINE__)
