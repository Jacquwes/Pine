#pragma once

#include <coroutine>
#include <thread>
#include <stdexcept>
#include <type_traits>

inline auto switch_thread(std::jthread& out)
{
	struct awaitable
	{
		std::jthread* p_out;
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			std::jthread& out = *p_out;
			if (out.joinable())
				throw std::runtime_error("Output jthread parameter not empty");
			out = std::jthread([h] { h.resume(); });
		}
		void await_resume() {}
	};
	return awaitable{ &out };
}

template <typename T>
	requires (!std::is_void_v<T>)
struct async_operation
{
	struct promise_type
	{
		T _value;
		async_operation<T> get_return_object() noexcept { return async_operation<T>(std::coroutine_handle<promise_type>::from_promise(*this)); }
		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		void unhandled_exception() const {}
		void return_value(T value) { _value = value; }
	};

	bool await_ready() const { return false; }
	void await_suspend(std::coroutine_handle<> h) const { h.resume(); }
	auto await_resume() { return _coro.promise()._value; }

	std::coroutine_handle<promise_type> _coro = nullptr;

	async_operation() = default;
	explicit async_operation(std::coroutine_handle<promise_type> coro) : _coro(coro) {}
	async_operation(async_operation const&) = delete;
	async_operation(async_operation&& other) noexcept
		: _coro(other._coro)
	{
		other._coro = nullptr;
	}
	~async_operation()
	{
		if (_coro.address()) _coro.destroy();
	}

	async_operation& operator=(async_operation&& other) noexcept
	{
		if (&other != this)
		{
			_coro = other._coro;
			other._coro = nullptr;
		}
	}
};

struct async_task
{
	struct promise_type
	{
		async_task get_return_object() const noexcept { return {}; }
		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		void unhandled_exception() const {}
		void return_void() const {}
	};

	bool await_ready() const noexcept { return false; }
	void await_suspend(std::coroutine_handle<> h) const noexcept { h.resume(); }
	void await_resume() const noexcept {}
};
