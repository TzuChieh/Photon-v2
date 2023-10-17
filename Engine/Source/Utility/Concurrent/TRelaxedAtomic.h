#pragma once

#include <atomic>
#include <type_traits>

namespace ph
{

/*! @brief Wrapper for relaxed atomic read and write. 
May resort to lock based read/write if atomic read/write for type @p T is not supported.
*/
template<typename T>
class TRelaxedAtomic final
{
public:
	TRelaxedAtomic()
		: m_atomic(T())
	{}

	TRelaxedAtomic(T value)
		: m_atomic(value)
	{}

	/*!
	@note Prefixing the method with the word "relaxed" is intended--it is to emphasize the
	fact that the read operation is with relaxed memory order, just like `std::memory_order_relaxed`
	would be specified at the call site if `std::atomic` was used directly.
	*/
	T relaxedRead() const
	{
		return m_atomic.load(std::memory_order_relaxed);
	}

	/*!
	@note Prefixing the method with the word "relaxed" is intended--it is to emphasize the
	fact that the write operation is with relaxed memory order, just like `std::memory_order_relaxed`
	would be specified at the call site if `std::atomic` was used directly.
	*/
	void relaxedWrite(T value)
	{
		m_atomic.store(value, std::memory_order_relaxed);
	}

	T relaxedFetchAdd(T value) requires std::is_integral_v<T> && std::is_floating_point_v<T>
	{
		return m_atomic.fetch_add(value, std::memory_order_relaxed);
	}

private:
	std::atomic<T> m_atomic;
};

}// end namespace ph
