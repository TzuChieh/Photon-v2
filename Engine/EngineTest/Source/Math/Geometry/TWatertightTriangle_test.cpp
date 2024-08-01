#include <Math/Geometry/TWatertightTriangle.h>
#include <Math/Geometry/TLineSegment.h>
#include <Math/TVector3.h>

#include <gtest/gtest.h>

#include <cmath>

using namespace ph;

TEST(TWatertightTriangleTest, IsDegenerate)
{
	{
		const math::TWatertightTriangle<float> rightTriangle(
			{0, 0, 0},
			{1, 0, 0},
			{0, 1, 0});

		EXPECT_FALSE(rightTriangle.isDegenerate());
	}

	{
		const math::TWatertightTriangle<float> triangle(
			{0, 0, 0},
			{10000.0f, 50000.0f,  90000.0f},
			{70000.0f, 30000.0f, -10000.0f});

		EXPECT_FALSE(triangle.isDegenerate());
	}

	if constexpr(std::numeric_limits<float>::has_infinity)
	{
		const math::TWatertightTriangle<float> triangle(
			{0, 0, 0},
			{1, 0, 0},
			{0, 1, std::numeric_limits<float>::infinity()});

		EXPECT_TRUE(triangle.isDegenerate());
	}

	if constexpr(std::numeric_limits<float>::has_quiet_NaN)
	{
		const math::TWatertightTriangle<float> triangle(
			{0, 0, 0},
			{1, 0, std::numeric_limits<float>::quiet_NaN()},
			{0, 1, 0});

		EXPECT_TRUE(triangle.isDegenerate());
	}

	if constexpr(std::numeric_limits<float>::has_signaling_NaN)
	{
		const math::TWatertightTriangle<float> triangle(
			{0, 0, std::numeric_limits<float>::signaling_NaN()},
			{1, 0, 0},
			{0, 1, 0});

		EXPECT_TRUE(triangle.isDegenerate());
	}
}

TEST(TWatertightTriangleTest, TolerateNonFiniteVertices)
{
	if constexpr(std::numeric_limits<float>::has_infinity)
	{
		const math::TWatertightTriangle<float> triangle(
			{0, 0, 0},
			{1, 1, 0},
			{1, -1, std::numeric_limits<float>::infinity()});

		// This segment will intersect the triangle if Inf is treated like a value >= 0
		const math::TLineSegment<float> segment(
			{0.1f, 0, -1},
			{0.1f, 0, 1});

		float t;
		math::TVector3<float> bary;
		EXPECT_FALSE(triangle.isIntersecting(segment, &t, &bary));
	}
}
