#include "constants_for_test.h"

#include <Common/primitive_type.h>
#include <Math/Function/TGaussian2D.h>
#include <Math/Function/TConstant2D.h>
#include <Math/Function/TMNCubic2D.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;

TEST(MathFunctionTest, TConstant2dHasCorrectProperty)
{
	const float32 constant = 1.7f;
	auto constantFunc = std::make_unique<TConstant2D<float32>>(constant);

	// trial 1: equals to specified value everywhere

	for(float32 y = -3.0f; y <= 3.0f; y += 1.2f)
	{
		for(float32 x = -3.0f; x <= 3.0f; x += 1.2f)
		{
			const float32 value = constantFunc->evaluate(x, y);
			EXPECT_EQ(value, constant);
		}
	}
}

TEST(MathFunctionTest, TGaussian2dHasCorrectProperty)
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

	// trial 3: submerging

	auto submergedGaussianFunc = std::make_unique<TGaussian2D<float32>>(sigmaX, sigmaY, amplitude);
	submergedGaussianFunc->setSubmergeAmount(submergedGaussianFunc->evaluate(0.5f, 0.5f));
	EXPECT_EQ(submergedGaussianFunc->evaluate( 0.51f,  0.51f), 0.0f);
	EXPECT_EQ(submergedGaussianFunc->evaluate(-0.51f, -0.51f), 0.0f);
	EXPECT_EQ(submergedGaussianFunc->evaluate(-1.00f,  2.00f), 0.0f);
}

TEST(MathFunctionTest, TMNCubic2dHasCorrectProperty)
{
	const float32 b = 1.0f / 3.0f;
	const float32 c = 1.0f / 3.0f;
	auto mnCubic2dFunc = std::make_unique<TMNCubic2D<float32>>(b, c);

	EXPECT_NEAR(mnCubic2dFunc->evaluate( 2,  0), 0.0f, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(mnCubic2dFunc->evaluate( 3,  3), 0.0f, TEST_FLOAT32_EPSILON);

	EXPECT_GT(mnCubic2dFunc->evaluate( 0.0f, 0.0f), 0.0f);
	EXPECT_GT(mnCubic2dFunc->evaluate( 0.5f, 0.5f), 0.0f);
	EXPECT_GT(mnCubic2dFunc->evaluate(-1.0f, 1.0f), 0.0f);
	EXPECT_LT(mnCubic2dFunc->evaluate(-1.5f, 1.0f), 0.0f);
}