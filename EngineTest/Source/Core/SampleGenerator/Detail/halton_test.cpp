#include "util.h"

#include <Core/SampleGenerator/Detail/halton.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::detail::halton;

TEST(HaltonImplDetailTest, RadicalInverse)
{
	// Base-2 radical inverse is effectively a bit reverse plus 
	// a power-of-two division
	{
		const uint64 bits = 
			0b00000001'10101010'00000000'10000011'00000000'00000011'10000000'00000000;
		const uint64 reversedBits =
			0b00000000'00000001'11000000'00000000'11000001'00000000'01010101'10000000;

		PH_EXPECT_REAL_EQ(
			radical_inverse<2>(bits),
			reversedBits / 0x1p64_r);
	}

	// Base-17
	{
		constexpr uint64 BASE = 17;
		constexpr uint64 VALUE = 123456789101112;
		
		uint64 currentValue = VALUE;
		uint64 nonScaledRadicalInverse = 0;
		real scaler = 1.0_r;
		while(currentValue > 0)
		{
			const auto lastDigit = currentValue % BASE;
			const auto remainingValue = currentValue / BASE;

			nonScaledRadicalInverse = nonScaledRadicalInverse * BASE + lastDigit;
			scaler /= BASE;
			currentValue = remainingValue;
		}

		PH_EXPECT_REAL_EQ(
			radical_inverse<BASE>(VALUE),
			nonScaledRadicalInverse * scaler);
	}
}
