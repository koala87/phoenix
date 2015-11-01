/**
	�ڴ����	
	MARCO USE_MEM_POOL ǿ��ʹ��new.
	˵��:
	ԭ������ȫ�֣������ڣ�
	���� operator new(size_t size), void operator delete(void*) , void operator delete[](void*) . 
	�������б��������������е�new���������ᱻ�滻���������ư汾�Ļ����Ӿɵķ����������п��ܵ��¾ɵ�new��ʹ���µ�delete�����µ�new��ʹ�þɵ�delete��
	�������һ��ͷ�ļ�����new��delete������

	���ԣ�ֻ�����C���ڴ�ӿڡ�
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
