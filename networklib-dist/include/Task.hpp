#pragma once

#include <Windows.h>
#include <tuple>

#if defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt-gd.lib")
#elif defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-sgd.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib, "networklib-mt.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib, "networklib-mt-s.lib")
#endif

#ifndef TASK_HPP
#define TASK_HPP


template<class Class_T, typename Func_Re_T, typename ...Args>
class Task
{
	Func_Re_T(Class_T::*func_ptr_)(Args...);
	Class_T& class_ref_;
	std::tuple<Args...> arguments_;
public:
	Task(Class_T& c_ref) : class_ref_(c_ref)
	{
		func_ptr_ = nullptr;
	}
	Task(Func_Re_T(Class_T::*f)(Args...), Class_T& c_ref, std::tuple<Args...>&& t) : func_ptr_(f), class_ref_(c_ref), arguments_(t)
	{

	}
	~Task()
	{
		func_ptr_ = nullptr;
	}

	Task& operator=(const Task<Class_T, Func_Re_T, Args...>& t)
	{
		this->func_ptr_ = t.func_ptr_;
		this->arguments_ = t.arguments_;
	}

	std::tuple<Args...>& GetArguments()
	{
		return arguments_;
	}
};

#endif // !TASK_HPP
