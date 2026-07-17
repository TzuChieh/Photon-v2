#include <Engine/Math/noise.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TVector4.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(NoiseTest, PerlinIsZeroAtLatticePoints)
{
	EXPECT_FLOAT_EQ(noise::perlin(-3.0f), 0.0f);
	EXPECT_FLOAT_EQ(noise::perlin(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(noise::perlin(7.0f), 0.0f);

	EXPECT_FLOAT_EQ(noise::perlin(Vector2F(-3.0f, 5.0f)), 0.0f);
	EXPECT_FLOAT_EQ(noise::perlin(Vector3F(-3.0f, 5.0f, 7.0f)), 0.0f);
	EXPECT_FLOAT_EQ(noise::perlin(Vector4F(-3.0f, 5.0f, 7.0f, -11.0f)), 0.0f);
}

TEST(NoiseTest, PerlinIsContinuousAtLatticeBoundaries)
{
	constexpr float32 epsilon = 1e-5f;
	constexpr float32 tolerance = 1e-3f;

	EXPECT_NEAR(noise::perlin(2.0f - epsilon), 0.0f, tolerance);
	EXPECT_NEAR(noise::perlin(2.0f + epsilon), 0.0f, tolerance);

	EXPECT_NEAR(noise::perlin(Vector2F(2.0f - epsilon, -3.0f)), 0.0f, tolerance);
	EXPECT_NEAR(noise::perlin(Vector2F(2.0f + epsilon, -3.0f)), 0.0f, tolerance);

	EXPECT_NEAR(noise::perlin(Vector3F(2.0f - epsilon, -3.0f, 5.0f)), 0.0f, tolerance);
	EXPECT_NEAR(noise::perlin(Vector3F(2.0f + epsilon, -3.0f, 5.0f)), 0.0f, tolerance);

	EXPECT_NEAR(noise::perlin(Vector4F(2.0f - epsilon, -3.0f, 5.0f, -7.0f)), 0.0f, tolerance);
	EXPECT_NEAR(noise::perlin(Vector4F(2.0f + epsilon, -3.0f, 5.0f, -7.0f)), 0.0f, tolerance);
}
