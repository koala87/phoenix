/**
 * DynamicData , Save Data in using curl, Non Thread Safe. 
 * @author YuChun Zhang
 * @date 2014.04.18
 */

#include <string>
#include <vector>
#include <algorithm>
#include "ThreadW1Rn.h"
#pragma once

namespace yiqiding { namespace net{



class DynamicData
{
private:
	char		*data_;
	size_t		index_;
	size_t		length_;
	explicit DynamicData(const DynamicData &){};

public:
	DynamicData(size_t length = 0);
	~DynamicData();
	

	const char *getData() const { return data_;}
	size_t getLenth() const { return index_;}
	void write(const char *in_data , size_t size);
	void reset(){ index_ = 0;}
	void release();
};


template<class T>
class DynamicDataList
{
private:
	size_t writepos_;
	size_t length_;
	std::vector< T*> lst_;
	std::vector<std::string> keys_;

	yiqiding::MutexWR mr_;
public:
	
	DynamicDataList(size_t length , int value = 0):writepos_(0) , length_(length), keys_(length)
	{
		for (int i = 0 ; i< length ; i++)
		{
			lst_.push_back(new T(value));
		}
	}
	~DynamicDataList()
	{
		std::vector<T*>::iterator it = lst_.begin();
		while(it != lst_.end())
		{
			delete *it;
			it = lst_.erase(it);
		}
		lst_.clear();
		keys_.clear();
	}
	//@Non Thread Safe
	const T*  readData( const std::string& key)
	{
	
		if (length_ == 0)
			return NULL;
		std::vector<std::string>::iterator it = find(keys_.begin() , keys_.end() , key);
		int index = (int)(it - keys_.begin());
		if (index < length_)
		{
			return lst_[index];
		}
		return NULL;		
	}	
	//@Non Thread Safe , using with finishLastData
	T* startLastData(const std::string &key)
	{
		T *t = NULL;
		if (length_ == 0)
			return t;
		keys_[writepos_] = key;
		t = lst_[writepos_];
		return t;
	}
	//@Non Thread Safe , using with startLastData
	void finishLastData()
	{
		++writepos_;
		if (writepos_ == length_)
		{
			writepos_ = 0;
		}
	}

	//@Thread Safe
	bool readData(const std::string &key , T &data){ 
		MutextReader wr(mr_); 
		if (length_ == 0)
			return false;
		std::vector<std::string>::iterator it = find(keys_.begin() , keys_.end() , key);
		int index = (int)(it - keys_.begin());
		if (index < length_)
		{
			data.write(lst_[index]->getData() , lst_[index]->getLenth());
			return true;
		}
		return false;
	}
	void writeData(const std::string &key , const T &data)
	{
		MutextReader wr(mr_); 
		if (length_ == 0)
			return;
		if(readData(key) != NULL)
			return;

		lst_[writepos_]->write(data.getData() , data.getLenth());
		keys_[writepos_] = key;

		++writepos_;
		if (writepos_ == length_)
		{
			writepos_ = 0;
		}

	}
};


} }
