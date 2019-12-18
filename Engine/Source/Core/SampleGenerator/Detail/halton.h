#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Math/math_table.h"
#include "Math/Random/shuffle.h"

#include <vector>
#include <utility>

namespace ph::detail::halton
{

template<uint64 BASE>
inline real radical_inverse(const uint64 value)
{
	static_assert(BASE >= 2);

	if constexpr(BASE == 2)
	{
		return math::reverse_bits(value) * 0x1p-64_r;
	}
	else
	{
		constexpr real rcpBase = 1.0_r / BASE;

		uint64 currentValue          = value;
		uint64 unscaledInversedValue = 0;
		real   scaler                = 1.0_r;
		while(currentValue > 0)
		{
			const uint64 quotient  = currentValue / BASE;
			const uint64 remainder = currentValue - quotient * BASE;

			unscaledInversedValue = unscaledInversedValue * BASE + remainder;
			scaler *= rcpBase;

			currentValue = quotient;
		}

		return std::clamp(unscaledInversedValue * scaler, 0.0_r, 1.0_r);
	}
}

template<uint64 BASE, typename P>
inline real radical_inverse_permuted(const uint64 value, const P* const permutation)
{
	static_assert(BASE >= 2);

	PH_ASSERT(permutation);

	constexpr real rcpBase = 1.0_r / BASE;

	uint64 currentValue          = value;
	uint64 unscaledInversedValue = 0;
	real   scaler                = 1.0_r;
	while(currentValue > 0)
	{
		const uint64 quotient          = currentValue / BASE;
		const uint64 remainder         = currentValue - quotient * BASE;
		const uint64 permutedRemainder = permutation[remainder];

		unscaledInversedValue = unscaledInversedValue * BASE + permutedRemainder;
		scaler *= rcpBase;

		currentValue = quotient;
	}

	// The permutation table may map the digit (remainder) 0 to a nonzero 
	// value. The while loop cannot catch this case since the iteration stops 
	// prematurely once <currentValue> becomes 0, missing an infinitely long 
	// suffix of digits with value <permutation[0]>. Fortunately, as stated in
	// the book of pbrt-v3 this is a geometric series with a simple analytic 
	// solution whose value can be added here. 
	// Note that <permutation[0] * rcpBase> is the first term of the geometric
	// series and <rcpBase> is the common ratio.
	return std::clamp(
		(unscaledInversedValue + permutation[0] * rcpBase / (1.0_r - rcpBase)) * scaler, 
		0.0_r, 1.0_r);
}

}// end namespace ph::detail::halton
