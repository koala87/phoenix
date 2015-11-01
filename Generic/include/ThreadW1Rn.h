/**
 * read n write 1.
 * @author YuChun Zhang
 * @date 2014.04.18
 */
#include "Thread.h"

#pragma once


namespace yiqiding{

class MutexReadWrite
{
public:
	virtual void writeLock() = 0;
	virtual void writeUnLock() = 0;
	virtual void readLock() = 0;
	virtual void readUnLock() = 0;
};




//读优先
class MutexRW:public MutexReadWrite
{
	yiqiding::Mutex						mr_;
	yiqiding::Mutex						mw_;
	size_t								rc_;
public:
	MutexRW():rc_(0){}
	void wirteLock() { mw_.lock();}
	void writeUnLock(){ mw_.unlock();}

	void readLock(){ 
		mr_.lock();
		if(rc_ == 0)
		{
			mw_.lock();
		}
		rc_++;
		mr_.unlock();
	 }
	void readUnLock(){
		mr_.lock();
		rc_--;
		if (rc_ == 0)
		{
			mw_.unlock();
		}
		mr_.unlock();
	}
};

//写优先
class MutexWR:public MutexReadWrite
{
	yiqiding::Mutex						mr_;
	yiqiding::Mutex						mnr_;
	yiqiding::Mutex						mrw_;
	size_t								rc_;
public:	
	MutexWR():rc_(0){}
	void writeLock(){ mrw_.lock(); mnr_.lock(); }
	void writeUnLock(){ mnr_.unlock(); mrw_.unlock();}

	void readLock(){ 
		mrw_.lock();
		mr_.lock();
		
		if (rc_ == 0)
		{
			mnr_.lock();
		}
		rc_++;
		mr_.unlock();
		mrw_.unlock();
	}
	void readUnLock(){
		mr_.lock();
		rc_--;
		if (rc_ == 0)
		{
			mnr_.unlock();
		}
		mr_.unlock();
	}
};

class MutextReader
{
	MutexReadWrite &rw_;
public:
	MutextReader(MutexReadWrite &rw):rw_(rw){ rw.readLock();}
	~MutextReader(){ rw_.readUnLock();}
};

class MutexWriter
{
	MutexReadWrite &rw_;
public:
	MutexWriter(MutexReadWrite &rw):rw_(rw){ rw.writeLock(); }
	~MutexWriter(){rw_.writeUnLock();}
};
}