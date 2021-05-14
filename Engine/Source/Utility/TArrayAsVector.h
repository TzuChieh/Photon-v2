#pragma once

#include <cstddef>
#include <array>

namespace ph
{

// TODO: dtor call policy? currently no dtor call on element remove, rely on life-time of vector

template<typename T, std::size_t N>
class TArrayAsVector final
{
public:
	TArrayAsVector();
	TArrayAsVector(const TArrayAsVector& other);
	~TArrayAsVector() = default;

	void pushBack(T&& item);
	void popBack();
	T& front();
	const T& front() const;
	T& back();
	const T& back() const;
	std::size_t size() const;
	void clear();
	bool isEmpty() const;

	TArrayAsVector& operator = (const TArrayAsVector& rhs);
	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

private:
	std::array<T, N> m_data;
	std::size_t      m_size;// TODO: size type based on N?
};

}// end namespace ph

#include "Utility/TArrayAsVector.ipp"
