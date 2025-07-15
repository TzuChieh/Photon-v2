#pragma once

#include "Engine/Math/Random/Random.h"
#include "Engine/Math/Random/sample.h"

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
	return ranged_pick(lowerBound, upperBound, sample());
}

}// end namespace ph::math
