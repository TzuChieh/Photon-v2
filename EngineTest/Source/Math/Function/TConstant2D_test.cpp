#include "constants_for_math_test.h"

#include <Common/primitive_type.h>
#include <Math/Function/TConstant2D.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;

TEST(MathOperationsTConstant2D, HasCorrectProperty)
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