#pragma once

#include "Math/TArithmeticArray.h"

#include <cmath>

namespace ph
{

template<typename T, std::size_t N>
inline std::size_t TArithmeticArray<T, N>::numElements()
{
	return N;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray() : 
	m()
{

}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const T value) : 
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
template<typename U>
inline TArithmeticArray<T, N>::TArithmeticArray(const TArithmeticArray<U, N>& other) : 
	TArithmeticArray()
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(other[i]);
	}
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::add(const TArithmeticArray& rhs) const
{
	return TArithmeticArray(*this).addLocal(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::add(const T rhs) const
{
	TArithmeticArray result(*this);
	for(std::size_t i = 0; i < N; ++i)
	{
		result.m[i] += rhs;
	}
	return result;
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
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::addLocal(const T rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] += rhs;
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
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::mul(const T rhs) const
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
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::mulLocal(const T rhs)
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
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::divLocal(const T rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] /= rhs;
	}

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::pow(const integer exponent) const
{
	TArithmeticArray result;
	for(std::size_t i = 0; i < N; ++i)
	{
		result.m[i] = std::pow(m[i], exponent);
	}

	return result;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::clampLocal(const T lowerBound, 
                                                                  const T upperBound)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = std::fmin(upperBound, std::fmax(m[i], lowerBound));
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
inline T TArithmeticArray<T, N>::avg() const
{
	return sum() / N;
}

template<typename T, std::size_t N>
inline T TArithmeticArray<T, N>::max() const
{
	T maxValue = m[0];
	for(std::size_t i = 1; i < N; ++i)
	{
		if(m[i] > maxValue)
		{
			maxValue = m[i];
		}
	}
	return maxValue;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::complement() const
{
	return TArithmeticArray(1).subLocal(*this);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::complementLocal()
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = 1 - m[i];
	}

	return *this;
}

template<typename T, std::size_t N>
template<typename U>
inline auto TArithmeticArray<T, N>::lerp(const TArithmeticArray& rhs, const U factor) const
	-> TArithmeticArray<U, N>
{
	TArithmeticArray<U, N> result;
	for(std::size_t i = 0; i < N; ++i)
	{
		result[i] = m[i] * (1 - factor) + rhs.m[i] * factor;
	}
	return result;
}

template<typename T, std::size_t N>
inline bool TArithmeticArray<T, N>::isZero() const
{
	// TODO: using lengthSquared() == 0 can achieve branchless isZero()
	// (will it be faster?)

	for(std::size_t i = 0; i < N; ++i)
	{
		if(m[i] != 0)
		{
			return false;
		}
	}

	return true;
}

template<typename T, std::size_t N>
inline bool TArithmeticArray<T, N>::isNonNegative() const
{
	for(std::size_t i = 0; i < N; ++i)
	{
		if(m[i] < 0)
		{
			return false;
		}
	}

	return true;
}

template<typename T, std::size_t N>
inline std::size_t TArithmeticArray<T, N>::size() const
{
	return N;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::set(const TArithmeticArray& other)
{
	m = other.m;

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::set(const T value)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = value;
	}

	return *this;
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

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::operator * (const T rhs) const
{
	return this->mul(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::operator + (const T rhs) const
{
	return this->add(rhs);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::operator = (const TArithmeticArray& rhs)
{
	m = rhs.m;

	return *this;
}

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::begin() noexcept
	-> typename std::array<T, N>::iterator
{
	return m.begin();
}

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::begin() const noexcept
	-> typename std::array<T, N>::const_iterator
{
	return m.cbegin();
}

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::end() noexcept
	-> typename std::array<T, N>::iterator
{
	return m.end();
}

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::end() const noexcept
	-> typename std::array<T, N>::const_iterator
{
	return m.cend();
}

}// end namespace ph