/**
 * Thread implementation based on pthread
 * @author Shiwei Zhang
 * @date 2014.01.17
 */

#include "Thread.h"
#include "Exception.h"
#include "utility/Logger.h"
using namespace yiqiding;

#pragma comment(lib,"pthreadVC2.lib")

//
// Mutex
//
Mutex::Mutex() {
	if (pthread_mutex_init(&_mutex, 0) != 0)
		throw Exception("pthread_mutex_init", __FILE__, __LINE__);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&_mutex);
}

void Mutex::lock() {
	if (pthread_mutex_lock(&_mutex) != 0)
		throw Exception("pthread_mutex_lock", __FILE__, __LINE__);
}

void Mutex::unlock() throw() {
	pthread_mutex_unlock(&_mutex);
}

//
// Condition
//
Condition::Condition() {
	if (pthread_cond_init(&_cond, 0) != 0)
		throw Exception("pthread_cond_init", __FILE__, __LINE__);
}

Condition::~Condition() {
	pthread_cond_destroy(&_cond);
}

void Condition::wait(Mutex &_mutex) {
	//MutexGuard _guard(_mutex);
	if (pthread_cond_wait(&_cond, &_mutex.native()) != 0)
		throw Exception("pthread_cond_wait", __FILE__, __LINE__);
}

void Condition::wait(Mutex &_mutex , int millsecond)
{
	timespec ts;
	ts.tv_sec = ::time(NULL) + millsecond / 1000;
	ts.tv_nsec = millsecond % 1000 * 1000;
	
	if(pthread_cond_timedwait(&_cond , &_mutex.native() , &ts) != 0)
		throw Exception("pthread_cond_wait" , __FILE__ , __LINE__);
}

void Condition::signal() {
	//MutexGuard _guard(_mutex);
	if (pthread_cond_signal(&_cond) != 0)
		throw Exception("pthread_cond_signal", __FILE__, __LINE__);
}

void Condition::broadcast() {
	//MutexGuard _guard(_mutex);
	if (pthread_cond_broadcast(&_cond) != 0)
		throw Exception("pthread_cond_broadcast", __FILE__, __LINE__);
}

//
//Semaphore
//

void Semaphore::wait()
{
	MutexGuard gurad(_mutex);
	while(_cur <= 0)
	{
		_cond.wait(_mutex);
	}
	if(_cur > 0)
		--_cur;
}

bool Semaphore::can()
{
	MutexGuard guard(_mutex);
	return _cur > 0;


}

void Semaphore::signal()
{
	MutexGuard gurad(_mutex);
	if(_count > _cur)
		_cond.signal();
	++_cur;
}	


//
// thread
//
void SelfDestruct::run() {
	if (_task != 0) {
		_task->run();
		delete _task;
	}
	delete this;
}

void Thread::start() {
	if (pthread_create(&_thread, 0, callback, this) != 0)
		throw Exception("pthread_create", __FILE__, __LINE__);
}

void Thread::join() {
	if (pthread_join(_thread, 0) != 0)
		throw Exception("pthread_join", __FILE__, __LINE__);
}

void Thread::detach() {
	if (pthread_detach(_thread) != 0)
		throw Exception("pthread_detach", __FILE__, __LINE__);
}

void Thread::exit() {
	pthread_exit(0);
}

void Thread::run() {
	if (_task != 0)
		_task->run();
}

void* Thread::callback(void* task) {

	//增加随机种子
	
	srand((unsigned int)time(NULL));
	((Thread*)task)->run();
	return 0;
}

//
// Thread Pool
//
ThreadWorker::ThreadWorker() : _loop(true) {
	start();
	detach();
}

bool ThreadWorker::perform(Runnable* task) {
	MutexGuard guard(_task_lock);
	if (_task == 0) {
		_task = task;
		_ready.signal();
		return true;
	} else
		return false;
}

void ThreadWorker::stop() {
	_loop = false;
	MutexGuard task_guard(_task_lock);
	_task = 0;
	_ready.signal();
}

void ThreadWorker::run() {
	while (_loop) {
		{
			MutexGuard task_guard(_task_lock);
			while (_task == 0)
				_ready.wait(_task_lock);
		}
		Thread::run();
		{
			MutexGuard task_guard(_task_lock);
			_task = 0;
		}
		if (_listener != 0)
			_listener->onDone(this);
	}
	if (_listener != 0)
		_listener->onStop(this);
}

void ThreadWorker::setListener(Listener* listener) {
	_listener = listener;
}

ThreadPool::ThreadPool(size_t min_threads, size_t max_threads) :
_min(min_threads), _max(max_threads) {
	for (size_t i = 0; i < _min; i++) {
		ThreadWorker* slave = new ThreadWorker;
		slave->setListener(this);
		_idle_workers.push_back(slave);
	}
}

ThreadPool::~ThreadPool() {
	join();
}

void ThreadPool::add(Runnable* task) {
	MutexGuard workers_guard(_workers_lock);
	if (!_idle_workers.empty()) {	// use idle workers
		ThreadWorker* slave = _idle_workers.back();
		_idle_workers.pop_back();
		_workers.push_back(slave);
		slave->perform(task);
	} else if (_workers.size() < _max || _max == 0) {	// more workers
		ThreadWorker* slave = new ThreadWorker;
		slave->setListener(this);
		_workers.push_back(slave);
		slave->perform(task);
		//yiqiding::utility::Logger::get("system")->log("thread pool new max" , yiqiding::utility::Logger::WARNING);
	} else {	// wait in the queue
		MutexGuard tasks_guard(_tasks_lock);
		_tasks.push(task);

		//yiqiding::utility::Logger::get("system")->log("thread pool wait" , yiqiding::utility::Logger::WARNING);
	}
}

void ThreadPool::onDone(ThreadWorker* slave) {
	{
		MutexGuard tasks_guard(_tasks_lock);
		if (!_tasks.empty()) {
			Runnable* task = _tasks.front();
			_tasks.pop();
			slave->perform(task);
			return;
		}
	}
	// no more tasks
	{
		MutexGuard workers_guard(_workers_lock);
		for (std::vector<ThreadWorker*>::iterator i = _workers.begin();
			i != _workers.end(); ++i)
			if (*i == slave) {
				_workers.erase(i);
				break;
			}
			if (_workers.size() < _min)
				_idle_workers.push_back(slave);
			else
				slave->stop();
			_all_done.signal();
	}
}

void ThreadPool::onStop(ThreadWorker* slave) {
	delete slave;
}

void ThreadPool::join() {
	MutexGuard workers_guard(_workers_lock);
	while (!(_workers.size() == 0 && _idle_workers.size() <= _min))
		_all_done.wait(_workers_lock);
}
