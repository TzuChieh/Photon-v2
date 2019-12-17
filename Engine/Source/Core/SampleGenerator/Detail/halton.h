#pragma once

#include "Common/primitive_type.h"
#include "Math/math.h"

namespace ph::halton_detail
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

}// end namespace ph::halton_detail
