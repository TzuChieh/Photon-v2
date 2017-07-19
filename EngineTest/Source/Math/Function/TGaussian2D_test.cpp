#include "constants_for_math_test.h"

#include <Common/primitive_type.h>
#include <Math/Function/TGaussian2D.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;

TEST(MathOperationsTGaussian2D, HasCorrectProperty)
{
	const float32 sigmaX = 1.2f;
	const float32 sigmaY = 1.6f;
	const float32 amplitude = 3.0f;
	auto gaussianFunc = std::make_unique<TGaussian2D<float32>>(sigmaX, sigmaY, amplitude);

	// trial 1: > 0 and < amplitude in [-3*sigmaX : 3*sigmaX, -3*sigmaY : 3*sigmaY]

	for(float32 y = -3.0f * sigmaY; y < 3.0f * sigmaY; y += 0.8f)
	{
		for(float32 x = -3.0f * sigmaX; x < 3.0f * sigmaX; x += 0.8f)
		{
			const float32 value = gaussianFunc->evaluate(x, y);
			EXPECT_GT(value, 0.0f);
			EXPECT_LE(value, amplitude);
		}
	}

	// trial 2: strictly increasing on y = 0.5, x in [-3*sigmaX, 0]

	for(float32 x = -3.0f * sigmaX; x < 0.0f; x += 0.5f)
	{
		const float32 value0 = gaussianFunc->evaluate(x - 0.25f, 0.5f);
		const float32 value1 = gaussianFunc->evaluate(x, 0.5f);
		EXPECT_LT(value0, value1);
	}
}