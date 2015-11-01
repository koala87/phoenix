/**
 * DynamicData , Save Data in using curl, Non Thread Safe. 
 * @author YuChun Zhang
 * @date 2014.04.18
 */

#include "net/DynamicData.h"
#include <string>

using namespace yiqiding::net;

DynamicData::DynamicData(size_t length):data_(NULL) , index_(0) , length_(0)
{
	if (length)
	{
		length_ = length;
		data_ = (char *)malloc(length);
	}
}

DynamicData::~DynamicData()
{
	release();
}

void DynamicData::write(const char *in_data , size_t size)
{
	if (in_data == NULL || size == 0 )
		return;

	if (index_ + size > length_)
	{
		size_t newlen = 2 * length_ >(index_ + size)? 2*length_ : (index_ + size); 		
		data_ = (char *)realloc(data_ , newlen);
		length_ = newlen;
	}

	memcpy(data_ + index_ , in_data , size);
	index_ += size;
}



void DynamicData::release()
{
	if (data_)
	{
		free(data_);
		data_ = NULL;
		length_ = 0;
		index_ = 0;
	}
}