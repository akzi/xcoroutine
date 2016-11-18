# xcoroutine
simple coroutine to fix the  async callback hell

```cpp
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
```
