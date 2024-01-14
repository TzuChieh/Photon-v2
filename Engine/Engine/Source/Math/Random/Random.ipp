#pragma once

#include "Math/Random/Random.h"

#include <Common/assertion.h>

namespace ph::math
{

template<std::size_t N, typename T>
inline std::array<T, N> Random::sampleND()
{
	static_assert(N != 0);

	std::array<T, N> values;
	for(std::size_t n = 0; n < N; ++n)
	{
		values[n] = static_cast<T>(sample());
	}
	return values;
}

inline std::size_t Random::index(const std::size_t lowerBound, const std::size_t upperBound)
{
	PH_ASSERT_GT(upperBound, lowerBound);

	const std::size_t numIntervals = upperBound - lowerBound;
	std::size_t index = static_cast<std::size_t>(lowerBound + sample() * numIntervals);

	return index < lowerBound ? lowerBound : (index >= upperBound ? upperBound - 1 : index);
}

}// end namespace ph::math
