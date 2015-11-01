#include "utility/memman.h"
#include "assert.h"
#include "Thread.h"
#include "malloc.h"
#include "net/DynamicData.h"


struct MemHeader
{
	const char*		data;
	MemHeader		*pre;
	MemHeader		*next;
	unsigned int	flag;
	unsigned int	bReturn;
	unsigned int	no;

};

template< size_t chunkSize , size_t Marker>
class MemMan
{
	struct MemNode
	{
		MemHeader header;
		unsigned char msg[chunkSize];
	};

	MemHeader _top;
	yiqiding::Mutex	_mutex;
	size_t	_available;
	size_t	_total;
	MemHeader _used;
public:
	MemMan(size_t init = 0): _available(0) , _total(0)
	{
		_top.next = _top.pre = &_top;
		_used.next = _used.pre = &_used;

		for(size_t i = 0 ; i < init ; ++i)
		{
			MemHeader *node = (MemHeader *)malloc(sizeof(MemNode));	
			node->flag = Marker;
			node->bReturn = 1;
			//in top
			node->pre = &_top;
			node->next =_top.next;
			_top.next->pre = node;
			_top.next = node;
			

			++_available;
			++_total;
		}


	}


	void * MemNew(size_t size , const char *data , unsigned int no)
	{
		yiqiding::MutexGuard guard(_mutex);
		if(_available != 0 )
		{
			//out node int top
			
			MemHeader *node = _top.next;
			node->pre->next = node->next;
			node->next->pre = node->pre;
			--_available;

			//in node in used
			node->pre = &_used;
			node->next =_used.next;
			_used.next->pre = node;	
			_used.next = node;
			

			node->bReturn =(unsigned int)( (size << 1) | 0);
			node->data = data;
			node->no = no;
			return node + 1;
		}
		else
		{
			MemHeader *node = (MemHeader *)malloc(sizeof(MemNode));	
			node->flag = Marker;
			node->bReturn =(unsigned int)((size << 1)|0);
			node->data = data;
			node->no = no;
			++_total;

			//in node in used
			node->pre = &_used;
			node->next =_used.next;
			_used.next->pre = node;
			_used.next = node;
			

			return node + 1;
		}
	}

	void MemFree(void *data)
	{
		yiqiding::MutexGuard guard(_mutex);
		MemHeader *node = (MemHeader *)((size_t)data - sizeof(MemHeader));
		assert((size_t)node > 0x00010000);
		assert(node->flag == Marker);
		assert((node->bReturn << 31) == 0);

		//out node in used
		node->pre->next = node->next;
		node->next->pre = node->pre;

		//in node in top 
		node->pre = &_top;
		node->next =_top.next;
		_top.next->pre = node;
		_top.next = node;
		
		node->data = NULL;
		node->no = 0;
		node->bReturn = 1;
		++_available;
	}

	void  MemShowAvailable(yiqiding::net::DynamicData &data)
	{
		yiqiding::MutexGuard guard(_mutex);

		MemHeader *node = _top.next;
		char buf[256];
		int len = sprintf_s(buf , "{\"size\":%d,\"count\":%d,\"items\":[" , chunkSize , _available);
		data.write(buf ,len);
		while(node != &_top)
		{
			assert(node->flag == Marker);
			len = sprintf_s(buf , "{\"address\":\"0x%x\",\"file\":\"%s\",\"line\":%d}" ,node + 1 , node->data , node->no);
			data.write(buf , len);
			node = node->next;
			if(node != &_top)
				data.write("," , 1);
		}

		data.write("]}" , 2);

	}

	void MemShowUsed(yiqiding::net::DynamicData &data)
	{
		yiqiding::MutexGuard guard(_mutex);
		MemHeader *node = _used.next;
		char buf[256];
		int len = sprintf_s(buf , "{\"size\":%d,\"count\":%d,\"items\":[" , chunkSize , _total - _available);
		data.write(buf ,len);
		while(node != &_used)
		{
			assert(node->flag == Marker);
			len = sprintf_s(buf , "{\"address\":\"0x%x\",\"file\":\"%s\",\"line\":%d, \"used\":%d}" ,node + 1 , node->data , node->no , node->bReturn >> 1);
			data.write(buf , len);
			node = node->next;
			if(node != &_used)
				data.write("," , 1);
		}
		data.write("]}" , 2);

	}
};


#define MEM_64_MARKER	0x1ffd1ffd
#define MEM_128_MARKER	0x2ffd2ffd
#define MEM_256_MARKER	0x3ffd3ffd
#define MEM_512_MARKER	0x4ffd4ffd
#define MEM_1k_MARKER	0x5ffd5ffd
#define MEM_2k_MARKER	0x6ffd6ffd
#define MEM_4k_MARKER	0x7ffd7ffd
#define MEM_8k_MARKER	0x8ffd8ffd
#define MEM_16k_MARKER	0x9ffd9ffd
#define MEM_32k_MARKER	0xaffdaffd
#define MEM_64k_MARKER	0xbffdbffd
#define MEM_128k_MARKER	0xcffdcffd
#define MEM_256k_MARKER	0xdffddffd
#define MEM_512k_MARKER	0xeffdeffd
#define MEM_1m_MARKER	0xfffdfffd
#define MEM_2m_MARKER	0x1ddf1ddf
#define MEM_4m_MARKER	0x2ddf1ddf

#define MEM_NONE_MARKER 0xfddffddf


class MemOtherMan
{
	size_t _total;
	MemHeader _used;
	yiqiding::Mutex _mutex;
public:
	MemOtherMan():_total(0)
	{
		_used.bReturn = 1;
		_used.flag = MEM_NONE_MARKER;
		_used.next = &_used;
		_used.pre = &_used;
	}

	void * MemNew(size_t size , const char *data , unsigned int no)
	{
		yiqiding::MutexGuard guard(_mutex);
		MemHeader *node = (MemHeader *)::malloc(sizeof(MemHeader) + size);
		node->bReturn = (unsigned int)((size << 1) | 0);
		node->data = data;
		node->no = no;
		node->flag = MEM_NONE_MARKER;

		
		//in used
		node->pre = &_used;
		node->next =_used.next;
		_used.next->pre = node;	
		_used.next = node;

		++_total;
		return node + 1;

	}
	void MemFree(void *data)
	{
		yiqiding::MutexGuard guard(_mutex);
		MemHeader *header = (MemHeader *)((size_t)data - sizeof(MemHeader));
		assert((header->bReturn << 31) == 0);
		assert(header->flag == MEM_NONE_MARKER);
		assert((size_t)header > 0x00010000);

		//out used
		header->pre->next = header->next;
		header->next->pre = header->pre;

		--_total;
		::free(header);
	}

	void MemShowUsed(yiqiding::net::DynamicData &data)
	{
		yiqiding::MutexGuard guard(_mutex);
		MemHeader *node = _used.next;
		char buf[256];
		int len = sprintf_s(buf , "{\"size\":%d,\"count\":%d,\"items\":[" , 0 , _total);
		data.write(buf ,len);
		while(node != &_used)
		{
			assert(node->flag == MEM_NONE_MARKER);
			len = sprintf_s(buf , "{\"address\":\"%x\",\"file\":\"%s\",\"line\":%d, \"used\":%d}" ,node + 1 , node->data , node->no , node->bReturn >> 1);
			data.write(buf , len);
			node = node->next;
			if(node != &_used)
				data.write("," , 1);
		}
		data.write("]}" , 2);
	}
};


class MemAll
{
	MemMan<64 , MEM_64_MARKER>			_mem64;
	MemMan<128 , MEM_128_MARKER>		_mem128;
	MemMan<256 , MEM_256_MARKER>		_mem256;
	MemMan<512 , MEM_512_MARKER>		_mem512;
	MemMan<1024 , MEM_1k_MARKER>		_mem1k;
	MemMan<2*1024 , MEM_2k_MARKER>		_mem2k;
	MemMan<4*1024 , MEM_4k_MARKER>		_mem4k;
	MemMan<8*1024 , MEM_8k_MARKER>		_mem8k;
	MemMan<16*1024 , MEM_16k_MARKER>	_mem16k;
	MemMan<32*1024 , MEM_32k_MARKER>	_mem32k;
	MemMan<64*1024 , MEM_64k_MARKER>	_mem64k;
	MemMan<128*1024 , MEM_128k_MARKER>	_mem128k;
	MemMan<256*1024 , MEM_256k_MARKER>	_mem256k;
	MemMan<512*1024 , MEM_512k_MARKER>	_mem512k;
	MemMan<1024*1024 , MEM_1m_MARKER>	_mem1m;
	MemMan<2*1024*1024 , MEM_2m_MARKER> _mem2m;
	MemMan<4*1024*1024 , MEM_4m_MARKER> _mem4m;
	MemOtherMan							_other;
	static MemAll						*_instance;

public:
	void * malloc(size_t size , const char *data  = NULL, unsigned int no = 0)
	{
		if(size <= 64)								return _mem64.MemNew(size , data , no);
		else if (size <= 128)						return _mem128.MemNew(size ,data , no);
		else if(size <= 256 ) 						return _mem256.MemNew(size , data , no);
		else if(size <= 512)						return _mem512.MemNew(size ,data , no);
		else if(size <= 1024)						return _mem1k.MemNew(size , data , no);
		else if(size <= 1024 *2)					return _mem2k.MemNew(size , data , no);
		else if(size <= 1024 *4)					return _mem4k.MemNew(size , data , no);
		else if(size <= 1024 *8)					return _mem8k.MemNew(size , data , no);
		else if(size <= 1024 *16)					return _mem16k.MemNew(size , data , no);
		else if(size <= 1024 *32)					return _mem32k.MemNew(size , data , no);
		else if(size <= 1024 *64)					return _mem64k.MemNew(size , data , no);
		else if(size <= 1024 *128)					return _mem128k.MemNew(size , data , no);
		else if(size <= 1024 *256)					return _mem256k.MemNew(size , data , no);
		else if(size <= 1024 *512)					return _mem512k.MemNew(size , data , no);
		else if(size <= 1024 *1024)					return _mem1m.MemNew(size , data , no);
		else if(size <= 1024 *1024 * 2)				return _mem2m.MemNew(size , data , no);
		else if(size <= 1024 *1024 * 4)				return _mem4m.MemNew(size , data , no);
		else										return _other.MemNew(size , data , no);

		
	}
	void free(void *data)
	{
		if(data == NULL)
			return;
		MemHeader *header = (MemHeader *)((size_t)data - sizeof(MemHeader));
		switch(header->flag)
		{
		case MEM_64_MARKER:return _mem64.MemFree(data);
		case MEM_128_MARKER:return _mem128.MemFree(data);
		case MEM_256_MARKER:return _mem256.MemFree(data);
		case MEM_512_MARKER:return _mem512.MemFree(data);
		case MEM_1k_MARKER:return _mem1k.MemFree(data);
		case MEM_2k_MARKER:return _mem2k.MemFree(data);
		case MEM_4k_MARKER:return _mem4k.MemFree(data);
		case MEM_8k_MARKER:return _mem8k.MemFree(data);
		case MEM_16k_MARKER:return _mem16k.MemFree(data);
		case MEM_32k_MARKER:return _mem32k.MemFree(data);
		case MEM_64k_MARKER:return _mem64k.MemFree(data);
		case MEM_128k_MARKER:return _mem128k.MemFree(data);
		case MEM_256k_MARKER:return _mem256k.MemFree(data);
		case MEM_512k_MARKER:return _mem512k.MemFree(data);
		case MEM_1m_MARKER:return _mem1m.MemFree(data);
		case MEM_2m_MARKER:return _mem2m.MemFree(data);
		case MEM_4m_MARKER:return _mem4m.MemFree(data);
		case MEM_NONE_MARKER:return _other.MemFree(data);
		default:	::free(data);
		}
	}
	void *calloc(size_t n , size_t size , const char *data = NULL , unsigned int no = 0)
	{
		void *ptr = malloc(n * size , data , no);
		memset(ptr , 0 , n * size);
		return ptr;
	}
	void* relloc(void *ptr , size_t size , const char *data = NULL , unsigned int no = 0)
	{
		size_t old_size = 0;
		MemHeader *header = (MemHeader *)((size_t)ptr - sizeof(MemHeader));

		old_size = header->bReturn >> 1;
		int oldk = (int)(old_size / 64);
		int newk = (int)(size / 64);
		if(newk > oldk)
		{
			void *newptr = malloc(size , data , no);
			memcpy(newptr , ptr , old_size);
			free(ptr);
			return newptr;
		}
		else
		{
			return ptr;
		}
		
	}
	void showAvailable(yiqiding::net::DynamicData &data)
	{
		char buf[256] = "{\"available\":[";
		data.write(buf , strlen(buf));
		_mem64.MemShowAvailable(data);
		data.write("," , 1);
		_mem128.MemShowAvailable(data);
		data.write("," , 1);
		_mem256.MemShowAvailable(data);
		data.write("," , 1);
		_mem512.MemShowAvailable(data);
		data.write("," , 1);
		_mem1k.MemShowAvailable(data);
		data.write("," , 1);
		_mem2k.MemShowAvailable(data);
		data.write("," , 1);
		_mem4k.MemShowAvailable(data);
		data.write("," , 1);
		_mem8k.MemShowAvailable(data);
		data.write("," , 1);
		_mem16k.MemShowAvailable(data);
		data.write("," , 1);
		_mem32k.MemShowAvailable(data);
		data.write("," , 1);
		_mem64k.MemShowAvailable(data);
		data.write("," , 1);
		_mem128k.MemShowAvailable(data);
		data.write("," , 1);
		_mem256k.MemShowAvailable(data);
		data.write("," , 1);
		_mem512k.MemShowAvailable(data);
		data.write("," , 1);
		_mem1m.MemShowAvailable(data);
		data.write("," , 1);
		_mem2m.MemShowAvailable(data);
		data.write("," , 1);
		_mem4m.MemShowAvailable(data);
		data.write("]}" , 2);
							
	}

	void sshowUsed(yiqiding::net::DynamicData &data)
	{
		char buf[256] = "{\"used\":[";
		data.write(buf , strlen(buf));
		_mem64.MemShowUsed(data);
		data.write("," , 1);		
		_mem128.MemShowUsed(data);
		data.write("," , 1);
		_mem256.MemShowUsed(data);
		data.write("," , 1);
		_mem512.MemShowUsed(data);
		data.write("," , 1);
		_mem1k.MemShowUsed(data);
		data.write("," , 1);
		_mem2k.MemShowUsed(data);
		data.write("," , 1);
		_mem4k.MemShowUsed(data);
		data.write("," , 1);
		_mem8k.MemShowUsed(data);
		data.write("," , 1);
		_mem16k.MemShowUsed(data);
		data.write("," , 1);
		_mem32k.MemShowUsed(data);
		data.write("," , 1);
		_mem64k.MemShowUsed(data);
		data.write("," , 1);
		_mem128k.MemShowUsed(data);
		data.write("," , 1);
		_mem256k.MemShowUsed(data);
		data.write("," , 1);
		_mem512k.MemShowUsed(data);
		data.write("," , 1);
		_mem1m.MemShowUsed(data);
		data.write("," , 1);
		_mem2m.MemShowUsed(data);
		data.write("," , 1);
		_mem4m.MemShowUsed(data);
		data.write("," , 1);
		_other.MemShowUsed(data);
		data.write("]}" , 2);
	}

	MemAll(int (&inits)[17] ):_mem64(inits[0]),_mem128(inits[1]),_mem256(inits[2]),_mem512(inits[3]),
		_mem1k(inits[4]),_mem2k(inits[5]),_mem4k(inits[6]),_mem8k(inits[7]),_mem16k(inits[8]),
		_mem32k(inits[9]),_mem64k(inits[10]),_mem128k(inits[11]),_mem256k(inits[12]),_mem512k(inits[13]),
		_mem1m(inits[14]),_mem2m(inits[15]),_mem4m(inits[16])
	{
		;
	}
	MemAll()
	{
		;
	}

	static MemAll *getInstance()
	{
		static yiqiding::Mutex _mutex;
		if(_instance == NULL)
		{
			yiqiding::MutexGuard guard(_mutex);
			if (_instance == NULL)
			{
				_instance = new (::malloc(sizeof(MemAll)))MemAll();
			}
		}

		return _instance;
	}

};

MemAll * MemAll::_instance = NULL;


void * smalloc(size_t size , const char *file ,  size_t no)
{
	return MemAll::getInstance()->malloc(size , file , (unsigned int)no);
}
void sfree(void *mem)
{
	return MemAll::getInstance()->free(mem);
}
void* scalloc(size_t n , size_t size , const char *file , size_t no)
{
	return MemAll::getInstance()->calloc(n , size , file , (unsigned int)no);
}
void * srealloc(void *mem_address , size_t size , const char *file , size_t no)
{
	return MemAll::getInstance()->relloc(mem_address ,size, file , (unsigned int)no);
}
void showAvailable(yiqiding::net::DynamicData &data)
{
	MemAll::getInstance()->showAvailable(data);

}
void showUsed(yiqiding::net::DynamicData &data)
{
	MemAll::getInstance()->sshowUsed(data);
}

#ifdef USE_MEM_POOL

void* operator new(size_t size, const char *file  , size_t no )
{
	return MemAll::getInstance()->malloc(size , file , (unsigned int)no);
}


void* operator new(size_t size)
{
	return  MemAll::getInstance()->malloc(size);
}

void operator delete(void* data)
{
	MemAll::getInstance()->free(data);
}

void operator delete[](void* data)
{
	MemAll::getInstance()->free(data);
}






#endif