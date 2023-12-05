#pragma once

#include "Math/General/TVectorNBase.h"
#include "Math/math.h"

#include <Common/assertion.h>

#include <cmath>

namespace ph::math
{

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::dot(const Derived& rhs) const
{
	T result(0);
	for(std::size_t i = 0; i < N; ++i)
	{
		result += m[i] * rhs.Self::m[i];
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::dot(const T rhs) const
{
	return sum() * rhs;
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::absDot(const Derived& rhs) const
{
	return std::abs(dot(rhs));
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::absDot(const T rhs) const
{
	return std::abs(dot(rhs));
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::length() const
{
	return std::sqrt(lengthSquared());
}

template<typename Derived, typename T, std::size_t N>
inline T TVectorNBase<Derived, T, N>::lengthSquared() const
{
	T result(0);
	for(std::size_t i = 0; i < N; ++i)
	{
		result += m[i] * m[i];
	}
	return result;
}

template<typename Derived, typename T, std::size_t N>
inline auto TVectorNBase<Derived, T, N>::normalize() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).normalizeLocal();
}

template<typename Derived, typename T, std::size_t N>
inline auto TVectorNBase<Derived, T, N>::normalizeLocal()
-> Derived&
{
	if constexpr(std::is_floating_point_v<T>)
	{
		PH_ASSERT_NE(length(), static_cast<T>(0));

		const T rcpLen = static_cast<T>(1) / length();
		for(std::size_t i = 0; i < N; ++i)
		{
			m[i] *= rcpLen;
		}
		return static_cast<Derived&>(*this);
	}
	else
	{
		static_assert(std::is_integral_v<T>);

		// The only way that normalizing an integer vector will not result in 
		// a 0-vector is that only a single component has finite value, while
		// all other components are zero. We detect these cases and directly
		// returns the result.

		constexpr auto EMPTY_IDX = static_cast<std::size_t>(-1);

		auto nonZeroIdx = EMPTY_IDX;
		for(std::size_t i = 0; i < N; ++i)
		{
			if(m[i] != static_cast<T>(0))
			{
				// Found the first non-zero component, record the index
				if(nonZeroIdx == EMPTY_IDX)
				{
					nonZeroIdx = i;
				}
				// This is not the first non-zero component, the result must be 0-vector
				else
				{
					set(static_cast<T>(0));
					return static_cast<Derived&>(*this);
				}
			}
		}

		// Every component is 0
		if(nonZeroIdx == EMPTY_IDX)
		{
			set(static_cast<T>(0));
		}
		// Only a single component is != 0
		else
		{
			PH_ASSERT_NE(m[nonZeroIdx], static_cast<T>(0));

			set(sign(m[nonZeroIdx]));
		}

		return static_cast<Derived&>(*this);
	}
}

template<typename Derived, typename T, std::size_t N>
template<typename>
inline auto TVectorNBase<Derived, T, N>::negate() const
-> Derived
{
	return Derived(static_cast<const Derived&>(*this)).negateLocal();
}

template<typename Derived, typename T, std::size_t N>
template<typename>
inline auto TVectorNBase<Derived, T, N>::negateLocal()
-> Derived&
{
	return mulLocal(static_cast<T>(-1));
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TVectorNBase<Derived, T, N>::minDimension() const
{
	// minIndex() is not exposed; use "this" to access it in current scope
	return this->minIndex();
}

template<typename Derived, typename T, std::size_t N>
inline std::size_t TVectorNBase<Derived, T, N>::maxDimension() const
{
	// maxIndex() is not exposed; use "this" to access it in current scope
	return this->maxIndex();
}

}// end namespace ph::math
