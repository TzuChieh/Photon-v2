#include <Common/assertion.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(AssertionTest, BasicAssertions)
{
	// These assertions should always pass; we are verifying that they compile
	// and execute correctly.
	PH_ASSERT(true);
	PH_ASSERT_MSG(1 + 1 == 2, "Math should work");

	// Equality and inequality assertions
	PH_ASSERT_EQ(10, 10);
	PH_ASSERT_NE(10, 20);

	// Relational assertions
	PH_ASSERT_GT(20, 10);
	PH_ASSERT_GE(20, 20);
	PH_ASSERT_LT(10, 20);
	PH_ASSERT_LE(10, 10);
}

TEST(AssertionTest, RangeAssertions)
{
	// Verifying inclusive and exclusive range assertions
	const int value = 5;

	// In range [0, 10)
	PH_ASSERT_IN_RANGE(value, 0, 10);

	// In range [5, 5] (inclusive)
	PH_ASSERT_IN_RANGE_INCLUSIVE(value, 5, 5);
	PH_ASSERT_IN_RANGE_INCLUSIVE(value, 0, 10);

	// In range (0, 10) (exclusive)
	PH_ASSERT_IN_RANGE_EXCLUSIVE(value, 0, 10);
}

TEST(AssertionTest, UnreachableCode)
{
	// This test simply verifies the macro can be called. 
	// In a real scenario, this would only be reached if logic failed.
	if (false)
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}
