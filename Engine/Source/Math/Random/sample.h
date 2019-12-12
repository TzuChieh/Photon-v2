#pragma once

#include "Common/assertion.h"
#include "Math/math.h"

namespace ph::math
{

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

}// end namespace ph::math
