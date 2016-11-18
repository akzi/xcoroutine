#include <string>
#include <iostream>
#include <functional>
#include <type_traits>
#include "../include/xcoroutine.hpp"
//#include "../../xutil/include/function_traits.hpp"
namespace xutil
{
	template<typename>
	struct function_traits;

	template<typename Ret, typename ...Args>
	struct function_traits<Ret(Args...)>
	{
		typedef std::function<Ret(Args...)> stl_function_type;
	};

	template<typename Ret, typename ...Args>
	struct function_traits<Ret(*)(Args...)> :function_traits<Ret(Args...)> { };

	template<typename Ret, typename Class, typename ...Args>
	struct function_traits<Ret(Class::*)(Args...) const> :function_traits<Ret(Args...)> { };

	template<typename Ret, typename Class, typename ...Args>
	struct function_traits<Ret(Class::*)(Args...)> :function_traits<Ret(Args...)> { };

	template<typename Callable>
	struct function_traits :function_traits<decltype(&Callable::operator())> { };

	template<typename Ret, typename ...Args>
	struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)> { };
}
static inline void coroutine_do(std::function<void(std::function<void()>)> async_do)
{
	std::function<void()> resume_func;
	bool is_calback = false;
	async_do([&resume_func, &is_calback]() {
		is_calback = true;
		if (resume_func)
			resume_func();
	});
	if (!is_calback)
		xcoroutine::yield(resume_func);
}

template<typename Result,typename ...Args>
static inline 
typename std::enable_if<sizeof...(Args) == 0, Result>::type
	coroutine_do(std::function<void(std::function<void(Result, Args...)>)> async_do)
{
	std::function<void()> resume_func;
	Result result;
	bool is_calback = false;
	async_do([&resume_func, &result, &is_calback](Result str) {
		result = std::move(str);
		is_calback = true;
		if(resume_func)
			resume_func();
	});
	if(!is_calback)
		xcoroutine::yield(resume_func);
	return std::move(result);
}

template<typename ...Args, typename Result = std::tuple<typename std::decay<Args>::type...>>
static inline
typename std::enable_if<sizeof...(Args) >= 2, Result>::type
	coroutine_do(std::function<void(std::function<void(Args...)>)> async_do)
{
	std::function<void()> resume_func;
	bool is_calback = false;
	Result result_;
	async_do([&resume_func, &result_, &is_calback](Args &&... agrs) {
		result_ = std::make_tuple(std::forward<Args>(agrs)...);
		is_calback = true;
		if (resume_func)
			resume_func();
	});
	if (!is_calback)
		xcoroutine::yield(resume_func);
	return std::move(result_);
}


template<typename FuncType>
static inline
typename xutil::function_traits<FuncType>::stl_function_type
	to_function(FuncType func)
{
	return  typename xutil::function_traits<FuncType>::stl_function_type{ func };
}


struct user
{
	user()
	{
		std::cout << "user()" << std::endl;
	}
	user(user &&)
	{
		std::cout << "user(user &&)" << std::endl;
	}
	user(const user &)
	{
		std::cout << "user(const user &)" << std::endl;
	}
	user operator= (const user &)
	{
		std::cout << "user operator= (const user &)" << std::endl;
		return *this;
	}
};
std::list<std::function<void(std::string)>> callbacks_;

std::function<void()> callback0_;
std::function<void(std::string)> callback1;
std::function<void(std::string &&, int)> callback2;
std::function<void(std::string &&, int, bool, user&&)> callbackN;

void async_do0(std::function<void()> callback)
{
	callback0_ = callback;
}
void async_do1(std::function<void(std::string)> callback)
{
	callback1 = callback;
}
void async_do2(std::function<void(std::string &&, int)> callback)
{
	callback2 = callback;
}
void async_doN(std::function<void(std::string &&, int, bool, user&&)> callback)
{
	callbackN = callback;
}

void xroutine_func1()
{
	coroutine_do(to_function(async_do0));

	auto result1 = coroutine_do(to_function(async_do1));
	assert(result1 == "hello world");

	auto result = coroutine_do(to_function(async_do2));
	assert(std::get<0>(result) == "hello world");
	assert(std::get<1>(result) == 1);

	auto resultN = coroutine_do(to_function(async_doN));
	assert(std::get<0>(resultN) == "hello world");
	assert(std::get<1>(resultN) == 1);
	assert(std::get<2>(resultN) == true);

}
void xtest()
{
	xcoroutine::create(xroutine_func1);

	callback0_();
	callback1("hello world");
	callback2("hello world", 1);
	callbackN("hello world", 1, true, user());
}
int main()
{
	xtest();
	return 0;
}
