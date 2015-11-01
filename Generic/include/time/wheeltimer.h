/*
基于linux 时间轮 的实现
*/
#pragma once
#include <cstdint>
#include "Thread.h"

namespace yiqiding { namespace time{
		
	const int		GRANULARITY		=		10;
	const int		WHEEL_BITS1		=		8;
	const int		WHEEL_BITS2		=		6;
	const int		WHEEL_SIZE1		=		(1 << WHEEL_BITS1);
	const int		WHEEL_SIZE2		=		(1 << WHEEL_BITS2);
	const int		WHEEL_MASK1		=		(WHEEL_SIZE1 - 1);
	const int		WHEEL_MASK2		=		(WHEEL_SIZE2 - 1);
	const int		WHEEL_NUM		=		5;

enum WheelTimer{
	WHEEL_ONESHOT,
	WHEEL_PERIODIC,
};

class CTimerManager;
class stWheel;


class stNodeLink:public yiqiding::Runnable
{
private:
	stNodeLink *		_prev;
	stNodeLink *		_next;	
	bool				_closed;				//closed 
	int					_thread;				//thread doing 
	bool				_stopped;
	uint32_t			_interval;
	uint32_t			_time;
	WheelTimer			_type;				
    Mutex	  			_mutex;

	bool tryRelease(bool flag);/* flag == true meaning in threading */
	void MarkAsThread() { ++_thread; }
	void run() { process(); if(tryRelease(true)) delete this; }
	stNodeLink() { _prev = _next = this;}
protected:
	
	uint32_t getInterval(){ return _interval;}
	WheelTimer getWheelTimer() { return _type;}
public:
	stNodeLink(uint32_t interval , WheelTimer type):_closed(false),_thread(0),_interval(interval),_type(type){_prev = _next = this;}
	virtual void process(){}
	virtual ~stNodeLink() { }
	friend CTimerManager;
	friend stWheel;
};





class stWheel
{
	stNodeLink *_spokes;
	uint32_t _size;
	uint32_t _spokeindex;
	stWheel(uint32_t n):_size(n),_spokeindex(0){
		_spokes = new stNodeLink[n];
	}
	~stWheel(){}
	friend CTimerManager;
};


class CTimerManager:public Thread
{
private:
	static CTimerManager	*_instance;
	static yiqiding::Mutex	_sMutex;

	stWheel				*_st[WHEEL_NUM];
	uint32_t			_base;
	ThreadPool			*_pool;
	uint32_t			_checktime;
	yiqiding::Mutex		_mutex;
	bool				_run;

private:	
	void addinternal(stNodeLink *st , uint32_t time);
	void detectTimeList();
	uint32_t Cascade(uint32_t wheelindex);
	void run();
	void start();
	void stop();
	CTimerManager(ThreadPool *pool):_pool(pool){}
	~CTimerManager();
	
public:
	
	
	
	//@Safe Thread 
	void add(stNodeLink *st);	
	void del(stNodeLink *st);

	static CTimerManager *getInstance(ThreadPool *_pool = NULL);
	static void unLoad();

	
	
};	

} }
