#pragma once

#include <coroutine>
#include <thread>
#include <stdexcept>
#include <type_traits>

/// @brief Continue execution on a different thread.
/// @param out The thread to continue execution on.
/// @return A coroutine awaitable.
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

/// @brief Continue execution on a new thread.
/// @return A coroutine awaitable.
inline auto switch_thread()
{
	struct awaitable
	{
		std::jthread thread;
		bool await_ready() { return false; }
		void await_suspend(std::coroutine_handle<> h)
		{
			thread = std::jthread([h] { h.resume(); });
		}
		void await_resume() {}
	};
	return awaitable{};
}

/// @brief An awaitable coroutine that returns a value.
/// @tparam T The type of the value to return.
/// @details For more information see: https://en.cppreference.com/w/cpp/language/coroutines
template <typename T>
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
	auto await_resume() { return _coroutine.promise()._value; }

	std::coroutine_handle<promise_type> _coroutine = nullptr;

	async_operation() = default;
	explicit async_operation(std::coroutine_handle<promise_type> coroutine) : _coroutine(coroutine) {}
	async_operation(async_operation const&) = delete;
	async_operation(async_operation&& other) noexcept
		: _coroutine(other._coroutine)
	{
		other._coroutine = nullptr;
	}
	~async_operation()
	{
		if (_coroutine.address()) _coroutine.destroy();
	}

	async_operation& operator=(async_operation&& other) noexcept
	{
		if (&other != this)
		{
			_coroutine = other._coroutine;
			other._coroutine = nullptr;
		}
	}
};

/// @brief An awaitable coroutine that returns nothing.
template <>
struct async_operation<void>
{
	struct promise_type
	{
		async_operation<void> get_return_object() const noexcept { return {}; }
		std::suspend_never initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		void unhandled_exception() const {}
		void return_void() const {}
	};

	bool await_ready() const { return false; }
	void await_suspend(std::coroutine_handle<> h) const { h.resume(); }
	void await_resume() { return; }

	std::coroutine_handle<promise_type> _coroutine = nullptr;

	async_operation() = default;
	explicit async_operation(std::coroutine_handle<promise_type> coroutine) : _coroutine(coroutine) {}
	async_operation(async_operation const&) = delete;
	async_operation(async_operation&& other) noexcept
		: _coroutine(other._coroutine)
	{
		other._coroutine = nullptr;
	}
	~async_operation()
	{
		if (_coroutine.address()) _coroutine.destroy();
	}

	async_operation& operator=(async_operation&& other) noexcept
	{
		if (&other != this)
		{
			_coroutine = other._coroutine;
			other._coroutine = nullptr;
		}
	}

};

/// @brief An awaitable coroutine that returns nothing.
using async_task = async_operation<void>;