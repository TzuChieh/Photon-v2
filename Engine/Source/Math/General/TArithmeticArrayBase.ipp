#pragma once

#include "Math/General/TArithmeticArrayBase.h"
#include "Common/assertion.h"

#include <cmath>
#include <utility>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
inline TArithmeticArrayBase<Derived, T, N>::TArithmeticArrayBase(const T value)
{
	m.fill(value);
}

template<typename Derived, typename T, std::size_t N>
inline TArithmeticArrayBase<Derived, T, N>::TArithmeticArrayBase(Elements values) :
	m(std::move(values))
{}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::add(const Derived& rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).addLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::add(const T rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).addLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::addLocal(const Derived& rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] += rhs.Self::m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::addLocal(const T rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] += rhs;
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::sub(const Derived& rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).subLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::sub(const T rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).subLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::subLocal(const Derived& rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] -= rhs.Self::m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::subLocal(const T rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] -= rhs;
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::mul(const Derived& rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).mulLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::mul(const T rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).mulLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::mulLocal(const Derived& rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] *= rhs.Self::m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::mulLocal(const T rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] *= rhs;
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::div(const Derived& rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).divLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::div(const T rhs) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).divLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::divLocal(const Derived& rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] /= rhs.Self::m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::divLocal(const T rhs)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] /= rhs;
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
template<typename Integer>
inline auto TArithmeticArrayBase<Derived, T, N>::pow(const Integer exponent) const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).powLocal(exponent);
}

template<typename Derived, typename T, std::size_t N>
template<typename Integer>
inline auto TArithmeticArrayBase<Derived, T, N>::powLocal(const Integer exponent)
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(std::pow(m[i], exponent));
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::sqrt() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).sqrtLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::sqrtLocal()
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(std::sqrt(m[i]));
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::clamp(const T lowerBound, const T upperBound)
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).clamp(lowerBound, upperBound);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::clampLocal(const T lowerBound, const T upperBound)
-> Derived&
{
	// TODO: properly handle integer types

	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = std::fmin(upperBound, std::fmax(m[i], lowerBound));
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline T TArithmeticArrayBase<Derived, T, N>::sum() const
{
	T result(0);
	for(std::size_t i = 0; i < N; ++i)
	{
		result += m[i];
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline T TArithmeticArrayBase<Derived, T, N>::avg() const
{
	if constexpr(std::is_floating_point_v<T>)
	{
		// Optimization for floating-point types: division by multiplication
		constexpr T rcpN = static_cast<T>(1) / static_cast<T>(N);

		return sum() * rcpN;
	}
	else
	{
		return sum() / static_cast<T>(N);
	}
}

template<typename Derived, typename T, std::size_t N>
inline T TArithmeticArrayBase<Derived, T, N>::product() const
{
	T result(1);
	for(std::size_t i = 0; i < N; ++i)
	{
		result *= m[i];
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline T TArithmeticArrayBase<Derived, T, N>::min() const
{
	return m[minIndex()];
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::min(const Derived& other) const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	for(std::size_t i = 0; i < N; ++i)
	{
		result.Self::m[i] = std::min(m[i], other.Self::m[i]);
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TArithmeticArrayBase<Derived, T, N>::minIndex() const
{
	std::size_t result = 0;
	for(std::size_t i = 1; i < N; ++i)
	{
		if(m[i] < m[result])
		{
			result = i;
		}
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline T TArithmeticArrayBase<Derived, T, N>::max() const
{
	return m[maxIndex()];
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::max(const Derived& other) const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	for(std::size_t i = 0; i < N; ++i)
	{
		result.Self::m[i] = std::max(m[i], other.Self::m[i]);
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TArithmeticArrayBase<Derived, T, N>::maxIndex() const
{
	std::size_t result = 0;
	for(std::size_t i = 1; i < N; ++i)
	{
		if(m[i] > m[result])
		{
			result = i;
		}
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::ceil() const
-> Derived
{
	if constexpr(std::is_floating_point_v<T>)
	{
		Derived result(static_cast<const Derived&>(*this));
		for(std::size_t i = 0; i < N; ++i)
		{
			result.Self::m[i] = static_cast<T>(std::ceil(m[i]));
		}
		return result;
	}
	else
	{
		return static_cast<const Derived&>(*this);
	}
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::floor() const
-> Derived
{
	if constexpr(std::is_floating_point_v<T>)
	{
		Derived result(static_cast<const Derived&>(*this));
		for(std::size_t i = 0; i < N; ++i)
		{
			result.Self::m[i] = static_cast<T>(std::floor(m[i]));
		}
		return result;
	}
	else
	{
		return static_cast<const Derived&>(*this);
	}
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::abs() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).absLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::absLocal()
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = std::abs(m[i]);
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::rcp() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).rcpLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::rcpLocal()
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(1) / m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::complement() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).complementLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::complementLocal()
-> Derived&
{
	for(std::size_t i = 0; i < N; ++i)
	{
		m[i] = static_cast<T>(1) - m[i];
	}
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
template<typename U>
inline auto TArithmeticArrayBase<Derived, T, N>::lerp(const Derived& rhs, const U factor) const
-> Derived
{
	Derived result(static_cast<const Derived&>(*this));
	for(std::size_t i = 0; i < N; ++i)
	{
		result.m[i] = m[i] * (static_cast<U>(1) - factor) + rhs.m[i] * factor;
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::isZero() const
{
	// TODO: using lengthSquared() == 0 can achieve branchless isZero()
	// (will it be faster?)

	for(std::size_t i = 0; i < N; ++i)
	{
		if(m[i] != static_cast<T>(0))
		{
			return false;
		}
	}
	return true;
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::isNonNegative() const
{
	if constexpr(std::is_unsigned_v<T>)
	{
		return true;
	}
	else
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
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::isFinite() const
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

template<typename Derived, typename T, std::size_t N>
inline constexpr std::size_t TArithmeticArrayBase<Derived, T, N>::size() const noexcept
{
	return NUM_ELEMENTS;
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::set(const T value)
-> Derived&
{
	m.fill(value);
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::set(const std::size_t index, const T value)
-> Derived&
{
	PH_ASSERT_LT(index, N);

	m[index] = value;
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::set(const std::array<T, N>& values)
-> Derived&
{
	m = values;
	return static_cast<Derived&>(*this);
}

template<typename Derived, typename T, std::size_t N>
inline T& TArithmeticArrayBase<Derived, T, N>::operator [] (const std::size_t index)
{
	PH_ASSERT_LT(index, N);

	return m[index];
}

template<typename Derived, typename T, std::size_t N>
inline const T& TArithmeticArrayBase<Derived, T, N>::operator [] (const std::size_t index) const
{
	PH_ASSERT_LT(index, N);

	return m[index];
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::isEqual(const Derived& other) const
{
	return m == other.Self::m;
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::isNear(const Derived& other, const T margin) const
{
	for(std::size_t i = 0; i < N; ++i)
	{
		if(std::abs(m[i] - other.Self::m[i]) > margin)
		{
			return false;
		}
	}
	return true;
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::operator == (const Derived& other) const
{
	return isEqual(other);
}

template<typename Derived, typename T, std::size_t N>
inline bool TArithmeticArrayBase<Derived, T, N>::operator != (const Derived& other) const
{
	return !isEqual(other);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator + (const Derived& rhs) const
-> Derived
{
	return add(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator + (const T rhs) const
-> Derived
{
	return add(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator - (const Derived& rhs) const
-> Derived
{
	return sub(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator - (const T rhs) const
-> Derived
{
	return sub(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator * (const Derived& rhs) const
-> Derived
{
	return mul(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator * (const T rhs) const
-> Derived
{
	return mul(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator / (const Derived& rhs) const
-> Derived
{
	return div(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator / (const T rhs) const
-> Derived
{
	return div(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator += (const Derived& rhs) const
-> Derived&
{
	return addLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator += (const T rhs) const
-> Derived&
{
	return addLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator -= (const Derived& rhs) const
-> Derived&
{
	return subLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator -= (const T rhs) const
-> Derived&
{
	return subLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator *= (const Derived& rhs) const
-> Derived&
{
	return mulLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator *= (const T rhs) const
-> Derived&
{
	return mulLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator /= (const Derived& rhs) const
-> Derived&
{
	return divLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::operator /= (const T rhs) const
-> Derived&
{
	return divLocal(rhs);
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::begin() noexcept
-> typename std::array<T, N>::iterator
{
	return m.begin();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::begin() const noexcept
-> typename std::array<T, N>::const_iterator
{
	return m.cbegin();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::end() noexcept
-> typename std::array<T, N>::iterator
{
	return m.end();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::end() const noexcept
-> typename std::array<T, N>::const_iterator
{
	return m.cend();
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::toString() const
-> std::string
{
	PH_ASSERT_NE(N, 0);

	std::string result("[");
	result += std::to_string(m[0]);
	for(std::size_t i = 1; i < N; ++i)
	{
		result += ", " + std::to_string(m[i]);
	}
	result += "]";

	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::toVector() const
-> std::vector<T>
{
	std::vector<T> vector(N);
	for(std::size_t i = 0; i < N; ++i)
	{
		vector[i] = m[i];
	}

	return vector;
}

template<typename Derived, typename T, std::size_t N>
inline auto TArithmeticArrayBase<Derived, T, N>::toArray() const
-> std::array<T, N>
{
	return m;
}

}// end namespace ph::math
