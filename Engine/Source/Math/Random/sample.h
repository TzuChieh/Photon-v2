#pragma once

#include "Common/assertion.h"
#include "Math/math.h"

namespace ph::math
{

template<typename T>
inline bool pick(const T sample, const T pickProbability)
{
	PH_ASSERT_IN_RANGE_INCLUSIVE(sample, T(0), T(1));// FIXME: should be half-opened range instead
	PH_ASSERT_IN_RANGE_INCLUSIVE(pickProbability, T(0), T(1));

	return sample < pickProbability;
}

template<typename T>
inline bool reused_pick(T& sample, const T pickProbability)
{
	// Note that <sample> is already on the correct scale if picking probability
	// is 0 or 1, hence the ternary conditionals; division by 0 is also handled.
	// TODO: check if division by 0 is actually impossible

	if(pick(sample, pickProbability))
	{
		sample = pickProbability != T(0) ? 
			math::clamp(sample / pickProbability, T(0), T(1)) : sample;

		return true;
	}
	else
	{
		sample = pickProbability != T(1) ? 
			math::clamp((sample - pickProbability) / (T(1) - pickProbability), T(0), T(1)) : sample;

		return false;
	}
}

}// end namespace ph::math
