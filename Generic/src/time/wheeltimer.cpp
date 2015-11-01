#include "time/wheeltimer.h"
#include <WinSock2.h>
#include <windows.h> 
#include <mmsystem.h>
#include "utility/Utility.h"
#include "utility/Logger.h"

using namespace yiqiding::time;


bool stNodeLink::tryRelease(bool flag)
{
	MutexGuard guard(_mutex);
	if(flag) // in threading
	{
		--_thread;
		if(_closed && !_thread)
			return true;
		else
			return false;
		
		 
	}
	else
	{
		if(_thread)
		{	
			_closed = true;
			return false;
		}
		else
			return true;
	}
}

CTimerManager * CTimerManager::_instance = NULL;
yiqiding::Mutex CTimerManager::_sMutex;

void CTimerManager::start()
{
	MutexGuard gurad(_mutex);
	_st[0] = new stWheel(WHEEL_SIZE1);
	_st[1] = new stWheel(WHEEL_SIZE2);
	_st[2] = new stWheel(WHEEL_SIZE2);
	_st[3] = new stWheel(WHEEL_SIZE2);
	_st[4] = new stWheel(WHEEL_SIZE2);

	_checktime = _base = ::timeGetTime();
	
	_run = true;
	Thread::start();	
}

void CTimerManager::stop()
{
	_run = false;
	Thread::join();
}

CTimerManager::~CTimerManager()
{
	stop();
	for (int i = 0 ; i < WHEEL_NUM ; ++i)
	{
		delete _st[i];
	}
}


void CTimerManager::add(stNodeLink *st)
{
	MutexGuard guard(_mutex);
	
	return addinternal(st , st->_interval + timeGetTime());
}

 

void CTimerManager::addinternal(stNodeLink *st , uint32_t time)
{
	stNodeLink *spoke = NULL;

	st->_time = time;
	uint32_t interval = (st->_time - timeGetTime())/ GRANULARITY;
	

	uint32_t threshold1 = WHEEL_SIZE1;
	uint32_t threshold2 = 1 << (WHEEL_BITS1 + WHEEL_BITS2);
	uint32_t threshold3 = 1 << (WHEEL_BITS1 + 2 * WHEEL_BITS2);
	uint32_t threshold4 = 1 << (WHEEL_BITS1 + 3 * WHEEL_BITS2);

	if (interval < threshold1) {
		uint32_t index = (interval + _st[0]->_spokeindex) & WHEEL_MASK1;
		spoke = _st[0]->_spokes + index;
	} else if (interval < threshold2) {
		uint32_t index = ((interval - threshold1 + _st[1]->_spokeindex * threshold1) >> WHEEL_BITS1) & WHEEL_MASK2;
		spoke = _st[1]->_spokes + index;
	} else if (interval < threshold3) {
		uint32_t index = ((interval - threshold2 + _st[2]->_spokeindex * threshold2) >> (WHEEL_BITS1 + WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = _st[2]->_spokes + index;
	} else if (interval < threshold4) {
		uint32_t index = ((interval - threshold3 + _st[3]->_spokeindex * threshold3) >> (WHEEL_BITS1 + 2 * WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = _st[3]->_spokes + index;
	} else {
		uint32_t index = ((interval - threshold4 + _st[4]->_spokeindex * threshold4) >> (WHEEL_BITS1 + 3 * WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = _st[4]->_spokes + index;
	}

	st->_prev = spoke->_prev;
	spoke->_prev->_next = st;
	st->_next = spoke;
	spoke->_prev = st;

	return ;
}



void CTimerManager::del(stNodeLink *st)
{

	MutexGuard guard(_mutex);

	st->_prev->_next = st->_next;
	st->_next->_prev = st->_prev;
	st->_prev = st->_next = NULL;
	
	if(st->tryRelease(false))	
		delete st;

}

CTimerManager * CTimerManager::getInstance(yiqiding::ThreadPool *_pool)
{
	if (_instance == NULL)
	{
		MutexGuard guard(_sMutex);
		if (_instance == NULL)
		{
			_instance = new CTimerManager(_pool);
			_instance->start();
		}

	}
	return _instance;
}

void CTimerManager::unLoad()
{
	MutexGuard guard(_sMutex);	
	if (_instance != NULL)
	{
		_instance->stop();
		delete _instance;
		_instance = NULL;
	}

}



void CTimerManager::detectTimeList()
{
	MutexGuard guard(_mutex);
	uint32_t now = timeGetTime();
	uint32_t loopnum = now > _checktime ? (now - _checktime ) / GRANULARITY :0;
	stWheel *wheel =  _st[0];
	for(uint32_t i = 0 ; i < loopnum ; ++i)
	{
		stNodeLink *spoke = wheel->_spokes + wheel->_spokeindex;
		stNodeLink *link = spoke->_next;
		stNodeLink *tmp ;
		//clear all
		spoke->_next = spoke->_prev = spoke;
		while(link != spoke){
			tmp = link->_next;
			
			//clear self. avoid del error
			link->_next = link->_prev = link;
			
			{
				link->MarkAsThread();
				_pool->add(link);
			}
			if(link->_type == WHEEL_PERIODIC)
			{
				addinternal(link , link->_time + link->_interval);
			}
			link = tmp;
		}
		if( ++wheel->_spokeindex >= wheel->_size)
		{
			wheel->_spokeindex = 0;
			Cascade(1);
		}
		_checktime += GRANULARITY;
	}
}

uint32_t CTimerManager::Cascade(uint32_t wheelindex)
{
	if (wheelindex < 1 || wheelindex >= WHEEL_NUM) {
		return 0;
	}
	stWheel *wheel =  _st[wheelindex];
	int casnum = 0;
	uint32_t now = ::timeGetTime();
	stNodeLink *spoke = wheel->_spokes + (wheel->_spokeindex++);
	stNodeLink *link = spoke->_next;
	stNodeLink *tmp;
	//clear all
	spoke->_next = spoke->_prev = spoke;
	while (link != spoke) {
		tmp = link->_next;
		
		//clear self. avoid del error
		link->_next = link->_prev = link;

		if (link->_time <= now) {
			{
				link->MarkAsThread();
				_pool->add(link);
			}
			if(link->_type == WHEEL_PERIODIC)
			{
				addinternal(link , link->_time + link->_interval);
			}
		} else {
			addinternal(link , link->_time);
			++casnum;
		}
		link = tmp;
	}

	if (wheel->_spokeindex >= wheel->_size) {
		wheel->_spokeindex = 0;
		casnum += Cascade(++wheelindex);
	}
	return casnum;
}

void CTimerManager::run()
{
	do{
		detectTimeList();
		Sleep(GRANULARITY);
	}while(_run);
}