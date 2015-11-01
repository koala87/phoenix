/**
 * Thread based on pthread
 * @author Shiwei Zhang
 * @date 2014.01.03
 */

#pragma once

#include <pthread.h>
#include <queue>
#include <vector>

namespace yiqiding {

	//
	// Mutex
	//
	class Mutex {
	private:
		pthread_mutex_t _mutex;
	public:
		Mutex();
		~Mutex();
		void lock();
		void unlock() throw();

		// inline functions
		inline pthread_mutex_t& native() { return _mutex; }
	};

	// misc tools
	class MutexGuard {
	private:
		Mutex& _lock;
	public:
		MutexGuard(Mutex& __lock) : _lock(__lock) { _lock.lock(); }
		~MutexGuard() { _lock.unlock(); }
	};

	//
	// Condition
	//
	class Condition {
	private:
		pthread_cond_t _cond;
		//Mutex		   _mutex;	
	public:
		Condition();
		~Condition();
		void wait(Mutex &_mutex);
		void wait(Mutex &_mutex , int millsecond);
		void signal();
		void broadcast();
	};
	
	class Semaphore{
		private:
			Condition		_cond;
			Mutex			_mutex;
			int				_cur;
			int				_count;
		
		public:
			Semaphore(int init = 1 , int count = 1):_cur(init),_count(count){}
			~Semaphore(){}
			void wait();
			void signal();
			bool can();
	};


	//
	// Thread
	//
	class Runnable {
	public:
		virtual ~Runnable() {};
		virtual void run() = 0;
	};
	typedef Runnable Task, ThreadTask;

	/// usage: new SelfDestruct(new Runnable).
	/// Provide automatic memory collection
	/// note: it will delete itself after executing run()
	class SelfDestruct : public Runnable {
	private:
		Runnable* _task;
	public:
		SelfDestruct(Runnable* task) : _task(task) {};
		void run();
	};

	class Thread {
	private:
		/// Thread handler 
		pthread_t _thread;
	protected:
		Runnable* _task;
	public:
		Thread(Runnable* task = NULL) : _task(task) {};
		virtual ~Thread() {};
		virtual void start();
		virtual void join();
		virtual void detach();
		static void exit();
	protected:
		virtual void run();
	private:
		static void* callback(void*);
	};

	//
	// Thread Pool
	//
	class ThreadWorker : private Thread {
	public:
		class Listener {
		public:
			virtual void onDone(ThreadWorker*) {}
			virtual void onStop(ThreadWorker*) {}
		};
	private:
		bool _loop;
		Mutex _task_lock;
		Condition _ready;
		Listener* _listener;
	public:
		ThreadWorker();
		~ThreadWorker() {}
		bool perform(Runnable* task);
		void stop();
		void setListener(Listener*);
	protected:
		void run();
	};

	class ThreadPool : public ThreadWorker::Listener {
	private:
		size_t _min;
		size_t _max;
		Mutex _tasks_lock;
		Mutex _workers_lock;
		Condition _all_done;
		std::queue<Runnable*> _tasks;
		std::vector<ThreadWorker*> _workers;
		std::vector<ThreadWorker*> _idle_workers;
	public:
		ThreadPool(size_t min_threads = 0, size_t max_threads = 0);
		~ThreadPool();
		void add(Runnable* task);
		void onDone(ThreadWorker*);
		void onStop(ThreadWorker*);
		void join();
	};
}
