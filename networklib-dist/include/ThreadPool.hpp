#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <memory>
#include <iostream>
#include <condition_variable>
#include <Task.hpp>

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-s.lib")
#endif

#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

//This Class Uses the Class References, Return Function Type, and Variadic Class Template for the function Parameters
template<class Class_T, typename Func_Re_T, typename ...Args>
class Thread_Pool
{
	std::vector<std::thread> pool;
	
	//system Information
	SYSTEM_INFO si;
	
	DWORD pool_limit;
	DWORD active_threads = 0;
public:
	std::queue<Task<Class_T, Func_Re_T, Args...>> queued_Tasks;

	//locking on tasks
	std::mutex task_lock;

	//locking on work
	std::mutex work_lock;

	//locking on threads
	std::mutex wake_lock;
	std::condition_variable wake_cond;


	//constructors
	Thread_Pool()
	{
		GetSystemInfo(&si);
		pool_limit = si.dwNumberOfProcessors;
	}
	Thread_Pool(Thread_Pool&) = delete;
	~Thread_Pool()
	{
		for (size_t idx = 0; idx < pool.size(); ++idx)
		{
			pool[idx].join();
		}
	}

	//Methods
	/*
		Method:		add_task
		Params:		Function Pointer, Class Reference Type, Arguments using Variadic Function Template
		Returns:	Void
		Purpose:	Add a Templated Task to the queue stack with the provided arguments. These objects are the tasks
						assigned or taken from the threads in the pool
	*/
	inline void add_task(Func_Re_T(Class_T::*f)(Args...), Class_T& c_ref, Args... args)
	{
		queued_Tasks.push(Task<Class_T, Func_Re_T, Args...>(f, c_ref, tuple<Args...>(args...)));
	}

	/*
		Method:		SetupPool
		Params:		Function Pointer, Class Reference Type, Arguments using Variadic Function Template
		Returns:	Void
		Purpose:	Uses the provided arguments to create a C++11 thread object to the thread pool with assigning it the
						provided method to run on, a class reference, and provided arguments.
	*/
	inline void SetupPool(Func_Re_T(Class_T::*f)(Args...), Class_T& c_ref, Args... args)
	{
		for (size_t idx = 0; idx < pool_limit; ++idx)
			pool.push_back(std::thread(f, &c_ref, std::ref(args)...));
	}
};

#endif // !Thread_Pool
