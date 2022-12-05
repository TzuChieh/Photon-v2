#pragma once

#include <atomic>
#include <utility>

namespace ph
{

template<typename T>
class TRelaxedRW final
{
public:
	inline TRelaxedRW()
		: m_atomic(T())
	{}

	template<typename U>
	inline TRelaxedRW(U&& value)
		: m_atomic(std::forward<U>(value))
	{}

	/*!
	@note Prefixing the method with the word "relaxed" is not redundant. It is to emphasize the
	fact that the read operation is with relaxed memory order, just like `std::memory_order_relaxed`
	would be specified at the call site if `std::atomic` was used directly.
	*/
	inline T relaxedRead() const
	{
		return m_atomic.load(std::memory_order_relaxed);
	}

	/*!
	@note Prefixing the method with the word "relaxed" is not redundant. It is to emphasize the
	fact that the write operation is with relaxed memory order, just like `std::memory_order_relaxed`
	would be specified at the call site if `std::atomic` was used directly.
	*/
	template<typename U>
	inline void relaxedWrite(U&& value)
	{
		m_atomic.store(std::forward<U>(value), std::memory_order_relaxed);
	}

private:
	std::atomic<T> m_atomic;
};

}// end namespace ph
