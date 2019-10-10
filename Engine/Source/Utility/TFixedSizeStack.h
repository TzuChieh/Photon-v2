#pragma once

#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TFixedSizeStack final
{
public:
	TFixedSizeStack();
	TFixedSizeStack(const TFixedSizeStack& other);
	~TFixedSizeStack() = default;

	void push(const T& item);
	void pop();
	T& top();
	const T& top() const;
	std::size_t height() const;
	void clear();
	bool isEmpty() const;

	TFixedSizeStack& operator = (const TFixedSizeStack& rhs);
	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

private:
	// must be signed type
	using Index = int;

	std::array<T, N> m_data;
	Index            m_currentIndex;
};

}// end namespace ph

#include "Utility/TFixedSizeStack.ipp"
