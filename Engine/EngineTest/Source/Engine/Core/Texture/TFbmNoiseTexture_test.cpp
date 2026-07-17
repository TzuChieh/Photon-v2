#include <Engine/Core/Texture/TFbmNoiseTexture.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Core/Texture/constant_textures.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TVector4.h>

#include <gtest/gtest.h>

#include <cmath>
#include <memory>

using namespace ph;
using namespace ph::math;

namespace
{

constexpr real MAX_ABS_ERROR = 2e-6_r;

real sampleFbm(const FbmNoiseTextureConfig& config, const SampleLocation& location)
{
	FbmNoiseTexture texture(config);

	real sample;
	texture.sample(location, &sample);
	return sample;
}

}// end namespace

TEST(FbmNoiseTextureTest, NormalizedLatticePointsHaveHalfValue)
{
	FbmNoiseTextureConfig config;
	config.scale = 1.0_r;
	config.detail = 0.0_r;
	const SampleLocation location(Vector3R(0), EColorUsage::Raw);

	auto expectHalfAt = [&config, &location](const int dimensions, const Vector4R& coordinates)
	{
		config.dimensions = dimensions;
		config.coordinatesMap = std::make_shared<TConstantTexture<Vector4R>>(coordinates);
		EXPECT_EQ(sampleFbm(config, location), 0.5_r)
			<< "dimensions=" << dimensions << ", coordinates=("
			<< coordinates.x() << ", " << coordinates.y() << ", " << coordinates.z() << ", " << coordinates.w() << ")";
	};

	constexpr int minCoordinate = -2;
	constexpr int maxCoordinate = 2;
	for(int x = minCoordinate; x <= maxCoordinate; ++x)
	{
		expectHalfAt(1, Vector4R(x, 0, 0, 0));
		for(int y = minCoordinate; y <= maxCoordinate; ++y)
		{
			expectHalfAt(2, Vector4R(x, y, 0, 0));
			for(int z = minCoordinate; z <= maxCoordinate; ++z)
			{
				expectHalfAt(3, Vector4R(x, y, z, 0));
				for(int w = minCoordinate; w <= maxCoordinate; ++w)
				{
					expectHalfAt(4, Vector4R(x, y, z, w));
				}
			}
		}
	}
}

TEST(FbmNoiseTextureTest, LargeCoordinatesProduceFiniteNormalizedOutput)
{
	FbmNoiseTextureConfig config;
	config.dimensions = 3;
	config.scale = 1.0_r;

	{
		const SampleLocation location(
			Vector3R(1e12_r, -1e12_r, 1e10_r), EColorUsage::Raw);
		const real sample = sampleFbm(config, location);

		EXPECT_TRUE(std::isfinite(sample));
		EXPECT_GE(sample, 0.0_r);
		EXPECT_LE(sample, 1.0_r);
	}

	{
		const SampleLocation location(
			Vector3R(1e30_r, -1e25_r, 1e20_r), EColorUsage::Raw);
		const real sample = sampleFbm(config, location);

		EXPECT_TRUE(std::isfinite(sample));
		EXPECT_GE(sample, 0.0_r);
		EXPECT_LE(sample, 1.0_r);
	}
}
