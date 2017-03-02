#pragma once

#include "Math/TArithmeticArray.h"

namespace ph
{

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray() : 
	m()
{

}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const T& value) : 
	m()
{
	m.fill(value);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const std::array<T, N>& values) : 
	m(values)
{

}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const TArithmeticArray& other) : 
	m(other.m)
{

}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::~TArithmeticArray() = default;

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::add(const TArithmeticArray& rhs) const
{
	return TArithmeticArray(*this).addLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::addLocal(const TArithmeticArray& rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] += rhs.m[i];
	}

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::sub(const TArithmeticArray& rhs) const
{
	return TArithmeticArray(*this).subLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::subLocal(const TArithmeticArray& rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] -= rhs.m[i];
	}

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::mul(const TArithmeticArray& rhs) const
{
	return TArithmeticArray(*this).mulLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::mul(const T& rhs) const
{
	return TArithmeticArray(*this).mulLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::mulLocal(const TArithmeticArray& rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] *= rhs.m[i];
	}

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::mulLocal(const T& rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] *= rhs;
	}

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::div(const TArithmeticArray& rhs) const
{
	return TArithmeticArray(*this).divLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::divLocal(const TArithmeticArray& rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] /= rhs.m[i];
	}

	return *this;
}

template<typename T, std::size_t N>
inline T TArithmeticArray<T, N>::dot(const TArithmeticArray& rhs) const
{
	T result(0);
	for(std::size_t i = 0; i < N; ++i)
	{
		result += m[i] * rhs.m[i];
	}

	return result;
}

template<typename T, std::size_t N>
inline T TArithmeticArray<T, N>::sum() const
{
	T result(0);
	for(std::size_t i = 0; i < N; ++i)
	{
		result += m[i];
	}

	return result;
}

template<typename T, std::size_t N>
inline T& TArithmeticArray<T, N>::operator [] (const std::size_t index)
{
	return m[index];
}

template<typename T, std::size_t N>
inline const T& TArithmeticArray<T, N>::operator [] (const std::size_t index) const
{
	return m[index];
}

}// end namespace ph