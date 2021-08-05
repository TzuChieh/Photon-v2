#pragma once

#include "Math/TArithmeticArray.h"
#include "Common/assertion.h"

#include <cmath>

namespace ph::math
{

template<typename T, std::size_t N>
inline std::size_t TArithmeticArray<T, N>::numElements()
{
	return N;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const T value)
{
	m.fill(value);
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>::TArithmeticArray(const std::array<T, N>& values) : 
	m(values)
{}

template<typename T, std::size_t N>
template<typename U>
inline TArithmeticArray<T, N>::TArithmeticArray(const TArithmeticArray<U, N>& other)
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
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::sub(const T rhs) const
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
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::subLocal(const T rhs)
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] -= rhs;
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
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::div(const T rhs) const
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
		result.m[i] = static_cast<T>(std::pow(m[i], exponent));
	}

	return result;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::sqrtLocal()
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = std::sqrt(m[i]);
	}

	return *this;
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
	return sum() / static_cast<T>(N);
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
inline TArithmeticArray<T, N> TArithmeticArray<T, N>::abs() const
{
	TArithmeticArray result;
	for(std::size_t i = 0; i < N; ++i)
	{
		result[i] = std::abs(m[i]);
	}
	return result;
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
inline bool TArithmeticArray<T, N>::isFinite() const
{
	for(std::size_t i = 0; i < N; ++i)
	{
		if(!std::isfinite(m[i]))
		{
			return false;
		}
	}

	return true;
}

template<typename T, std::size_t N>
inline constexpr std::size_t TArithmeticArray<T, N>::size() const noexcept
{
	return N;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::set(const T value)
{
	m.fill(value);

	return *this;
}

template<typename T, std::size_t N>
inline TArithmeticArray<T, N>& TArithmeticArray<T, N>::set(const std::array<T, N>& values)
{
	m = values;

	return *this;
}

template<typename T, std::size_t N>
inline T& TArithmeticArray<T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT(index < N);

	return m[index];
}

template<typename T, std::size_t N>
inline const T& TArithmeticArray<T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT(index < N);

	return m[index];
}

template<typename T, std::size_t N>
inline bool TArithmeticArray<T, N>::operator == (const TArithmeticArray& other) const
{
	for(std::size_t i = 0; i < N; ++i)
	{
		if(m[i] != other.m[i])
		{
			return false;
		}
	}
	return true;
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

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::toString() const
	-> std::string
{
	PH_ASSERT(N != 0);

	std::string result("[");
	result += std::to_string(m[0]);
	for(std::size_t i = 1; i < N; ++i)
	{
		result += ", " + std::to_string(m[i]);
	}
	result += "]";

	return result;
}

template<typename T, std::size_t N>
inline auto TArithmeticArray<T, N>::toVector() const
	-> std::vector<T>
{
	std::vector<T> vector(N);
	for(std::size_t i = 0; i < N; ++i)
	{
		vector[i] = m[i];
	}

	return vector;
}

}// end namespace ph::math
