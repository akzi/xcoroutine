#include <string>
#include <iostream>
#include <functional>
#include <type_traits>
#include "../include/xcoroutine.hpp"
#include "../../xutil/include/function_traits.hpp"

struct user
{
	user() {}
	user(int id)
		:id_(id) 
	{}
	int id_;
};

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
	using xutil::to_function;
	using xcoroutine::apply;

	apply(to_function(async_do0));

	auto result1 = apply(to_function([](std::function<void(std::string)> callback) { callback1 = callback; }));
	assert(result1 == "hello world");

	auto result = apply(to_function(async_do2));
	assert(std::get<0>(result) == "hello world");
	assert(std::get<1>(result) == 1);

	auto resultN = apply(to_function(async_doN));
	assert(std::get<0>(resultN) == "hello world");
	assert(std::get<1>(resultN) == 1);
	assert(std::get<2>(resultN) == true);
	assert(std::get<3>(resultN).id_ == 1);

}
void xtest()
{
	xcoroutine::create(xroutine_func1);

	callback0_();
	callback1("hello world");
	callback2("hello world", 1);
	callbackN("hello world", 1, true, user(1));
}
int main()
{
	xtest();
	return 0;
}
