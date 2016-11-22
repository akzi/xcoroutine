#include <string>
#include <iostream>
#include <functional>
#include <type_traits>
#include "../../xutil/include/function_traits.hpp"
#include "../include/xcoroutine.hpp"

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
void async_do1(const std::string &str, std::function<void(std::string)> callback)
{
	callback1 = callback;
}
void async_do2(const std::string &str, std::function<void(std::string &&, int)> callback)
{
	callback2 = callback;
}
void async_doN(const std::string &str, std::string && data, std::function<void(std::string &&, int, bool, user&&)> callback)
{
	callbackN = callback;
}
void xroutine_func2()
{
	using xutil::to_function;
	using xcoroutine::apply;
	apply(to_function(async_do0));
	auto res = apply(to_function(async_do1),"hello");
	assert(std::get<0>(res) == "hello world");

	auto res2 = apply(to_function(async_do2), "hello");
	assert(std::get<0>(res2) == "hello world");
	assert(std::get<1>(res2) == 1);

	auto res3 = apply(to_function(async_doN), "hello", "hello2");
	assert(std::get<0>(res3) == "hello world");
	assert(std::get<1>(res3) == 1);
	assert(std::get<2>(res3) == true);
	assert(std::get<3>(res3).id_ == 1);

}
int main()
{
	xcoroutine::create( xroutine_func2);


	callback0_();
	callback1("hello world");
	callback2("hello world", 1);
	callbackN("hello world", 1, true, user(1));
	return 0;
}
