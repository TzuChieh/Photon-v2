#pragma once

/*! @file

@brief Basic sampling routines.

A "sample" is expected to be uniformly and randomly distributed in [0, 1].
*/

#include "Engine/Math/math.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <cstddef>
#include <array>
#include <utility>
#include <type_traits>
#include <climits>
#include <optional>

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

/*! @brief Randomly pick a branch with some probability.
Same as `pick()`, except that `sample` will be updated for the next pick.
@param pickProbability The probability for the function to return `true`.
@param sample[inout] The source of randomness. Will be updated for the next pick.
@return `true` if the pick is successful.
*/
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

/*! @brief Get a uniform random index in [`lowerBound`, `upperBound`).
*/
template<typename T, typename Index>
inline Index ranged_pick(const Index lowerBound, const Index upperBound, const T sample)
{
	PH_ASSERT_GT(upperBound, lowerBound);

	const auto numIntervals = upperBound - lowerBound;
	const auto index        = static_cast<Index>(lowerBound + sample * numIntervals);

	return index < lowerBound ? lowerBound : (index >= upperBound ? upperBound - 1 : index);
}

/*! @brief Uniformly sample from a collection of indices.
This function can sample from a non-contiguous collection of indices, and without the need of
a container. The cost is that it performs random selection #indices times.
@tparam T Type of the probability to feed into `PickFunc`.
@tparam Index Type of the index.
@tparam PickFunc Invocable object to perform pick with a certain probability with signature `bool(T)`.
@tparam IndexFunc Invocable object to get an index with signature `std::optional<Index>(void)`.
Returning an empty index indicates the index stream ends.
@return A pair containing the selected index (an `std::optional`) and the total number of indices.
*/
template<typename T, typename Index, typename PickFunc, typename IndexFunc>
inline auto uniform_reservoir_pick(IndexFunc indexFunc, PickFunc pickFunc)
-> std::pair<std::optional<Index>, Index>
{
	using OptIndex = std::optional<Index>;

	static_assert(std::is_invocable_r_v<bool, PickFunc, T>);
	static_assert(std::is_invocable_r_v<OptIndex, IndexFunc>);

	OptIndex selected   = indexFunc();
	OptIndex next       = selected;
	Index    numIndices = 0;
	while(next.has_value())
	{
		++numIndices;

		const auto probability = 1 / T(numIndices);
		if(pickFunc(probability))
		{
			selected = next;
		}

		next = indexFunc();
	}
	return std::pair<std::optional<Index>, Index>{selected, numIndices};
}

/*! @brief Same as `uniform_reservoir_pick()`, just with a more friendly name.
*/
template<typename T, typename Index, typename PickFunc, typename IndexFunc>
inline auto uniform_pick(IndexFunc indexFunc, PickFunc pickFunc)
-> std::pair<std::optional<Index>, Index>
{
	return uniform_reservoir_pick<T, Index>(
		std::forward<IndexFunc>(indexFunc),
		std::forward<PickFunc>(pickFunc));
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
