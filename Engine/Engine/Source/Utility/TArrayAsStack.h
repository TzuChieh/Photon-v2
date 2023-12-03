#pragma once

#include <cstddef>
#include <array>

namespace ph
{

// TODO: dtor call policy? currently no dtor call on element remove, rely on life-time of vector

template<typename T, std::size_t N>
class TArrayAsStack final
{
public:
	TArrayAsStack();
	TArrayAsStack(const TArrayAsStack& other);
	~TArrayAsStack() = default;

	template<typename U>
	void push(U&& item);

	void pop();
	T& top();
	const T& top() const;
	std::size_t height() const;
	void clear();
	bool isEmpty() const;

	TArrayAsStack& operator = (const TArrayAsStack& rhs);
	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

private:
	// must be signed type
	using Index = int;

	std::array<T, N> m_data;
	Index            m_currentIndex;
};

}// end namespace ph

#include "Utility/TArrayAsStack.ipp"
