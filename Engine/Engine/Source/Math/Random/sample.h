#pragma once

/*! @file

@brief Basic sampling routines.

A "sample" is expected to be uniformly and randomly distributed in [0, 1].
*/

#include "Math/math.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <array>
#include <utility>
#include <type_traits>
#include <climits>

namespace ph::math
{

template<typename To, typename From, std::size_t N>
inline std::array<To, N> sample_cast(const std::array<From, N>& sample)
{
	std::array<To, N> casted;
	for(std::size_t i = 0; i < N; ++i)
	{
		casted[i] = static_cast<To>(sample[i]);
	}
	return casted;
}

/*! @brief Randomly pick a branch with some probability.
@param pickProbability The probability for the function to return `true`.
@param sample The source of randomness.
@return `true` if the pick is successful.
*/
template<typename T>
inline bool pick(const T pickProbability, const T sample)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample, T(0), T(1));
	PH_ASSERT_IN_RANGE_INCLUSIVE(pickProbability, T(0), T(1));

	return sample < pickProbability;
}

template<typename T>
inline bool reused_pick(const T pickProbability, T& sample)
{
	// Note that <sample> is already on the correct scale if picking probability
	// is 0 or 1, hence the ternary conditionals; division by 0 is also handled.

	if(pick(pickProbability, sample))
	{
		sample = pickProbability != T(0) ? 
			math::clamp(sample / pickProbability, T(0), T(1)) : sample;

		return true;
	}
	else
	{
		// The denominator should never be 0
		PH_ASSERT_NE(pickProbability != T(1) ? (T(1) - pickProbability) : T(1), T(0));

		sample = pickProbability != T(1) ? 
			math::clamp((sample - pickProbability) / (T(1) - pickProbability), T(0), T(1)) : sample;

		return false;
	}
}

/*! @brief Converts input bits to a sample.
For standard unsigned integral types, this is effectively normalizing the integer value represented
by `bits` into the range [0, 1].
@param bits The bits to convert.
*/
template<typename T, typename BitsType>
inline T bits_to_sample(const BitsType& bits)
{
	static_assert(CHAR_BIT == 8);

	T normalizer = static_cast<T>(0);
	if constexpr(std::is_same_v<BitsType, uint8>)
	{
		normalizer = static_cast<T>(0x1p-8);
	}
	else if constexpr(std::is_same_v<BitsType, uint16>)
	{
		normalizer = static_cast<T>(0x1p-16);
	}
	else if constexpr(std::is_same_v<BitsType, uint32>)
	{
		normalizer = static_cast<T>(0x1p-32);
	}
	else if constexpr(std::is_same_v<BitsType, uint64>)
	{
		normalizer = static_cast<T>(0x1p-64);
	}
	else
	{
		PH_STATIC_ASSERT_DEPENDENT_FALSE(T,
			"No existing implementation can do `BitsType` -> sample type `T`.");
	}

	const T sample = bits * normalizer;
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample, static_cast<T>(0.0), static_cast<T>(1.0));
	return sample;
}

}// end namespace ph::math
