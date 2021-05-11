#pragma once

#include <cstddef>
#include <array>

namespace ph
{

template<typename T, std::size_t N>
class TFixedSizeVector final
{
public:
	TFixedSizeVector();
	TFixedSizeVector(const TFixedSizeVector& other);
	~TFixedSizeVector() = default;

	void push(T item);
	void pop();
	std::size_t size() const;
	void clear();
	bool isEmpty() const;

	TFixedSizeVector& operator = (const TFixedSizeVector& rhs);
	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

private:
	std::array<T, N> m_data;
	std::size_t      m_size;// TODO: size type based on N?
};

}// end namespace ph

#include "Utility/TFixedSizeVector.ipp"
