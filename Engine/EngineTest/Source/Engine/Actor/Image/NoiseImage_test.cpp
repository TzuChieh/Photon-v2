#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Image/NoiseImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/Color/Spectrum.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/TVector4.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph;
using namespace ph::math;

TEST(NoiseImageTest, NormalizedFbmIsHalfAtLatticeOrigin)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	auto image = TSdl<NoiseImage>::make();
	const auto scalarTexture = image.genRealTexture(ctx);

	real scalar;
	scalarTexture->sample(SampleLocation(Vector3R(0), EColorUsage::Raw), &scalar);

	// Every octave is zero at a Perlin lattice origin; normalized fBM maps zero to 0.5.
	EXPECT_EQ(scalar, 0.5_r);
}

TEST(NoiseImageTest, ScalarResultConvertsToImageOutputTypes)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	auto image = TSdl<NoiseImage>::make();
	const auto scalarTexture = image.genRealTexture(ctx);
	const auto vector2Texture = image.genVector2RTexture(ctx);
	const auto vector3Texture = image.genVector3RTexture(ctx);
	const auto vector4Texture = image.genVector4RTexture(ctx);
	const auto numericTexture = image.genNumericTexture(ctx);
	const auto colorTexture = image.genColorTexture(ctx);
	const SampleLocation location(Vector3R(0.17_r, -0.31_r, 0.73_r), EColorUsage::Raw);

	real scalar;
	Vector2R vector2;
	Vector3R vector3;
	Vector4R vector4;
	Image::NumericType numeric;
	Spectrum color;
	scalarTexture->sample(location, &scalar);
	vector2Texture->sample(location, &vector2);
	vector3Texture->sample(location, &vector3);
	vector4Texture->sample(location, &vector4);
	numericTexture->sample(location, &numeric);
	colorTexture->sample(location, &color);
	const Spectrum expectedColor(static_cast<ColorValue>(scalar));

	EXPECT_EQ(vector2.x(), scalar);
	EXPECT_EQ(vector2.y(), 0.0_r);
	EXPECT_EQ(vector3.x(), scalar);
	EXPECT_EQ(vector3.y(), 0.0_r);
	EXPECT_EQ(vector3.z(), 0.0_r);
	EXPECT_EQ(vector4.x(), scalar);
	EXPECT_EQ(vector4.y(), 0.0_r);
	EXPECT_EQ(vector4.z(), 0.0_r);
	EXPECT_EQ(vector4.w(), 0.0_r);
	EXPECT_EQ(numeric[0], scalar);
	EXPECT_EQ(numeric[1], 0.0_r);
	EXPECT_EQ(numeric[2], 0.0_r);
	EXPECT_EQ(numeric[3], 0.0_r);
	EXPECT_EQ(color.getColorValues(), expectedColor.getColorValues());
}

TEST(NoiseImageTest, MappedCoordinatesOverrideSampleUvw)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);
	const Vector4R mappedCoordinates(0.17_r, -0.31_r, 0.73_r, 0.0_r);
	auto coordinates = TSdl<ConstantImage>::makeResource();
	coordinates->setRaw(std::vector<real>{
		mappedCoordinates.x(),
		mappedCoordinates.y(),
		mappedCoordinates.z(),
		mappedCoordinates.w()});

	auto mappedImage = TSdl<NoiseImage>::make();
	mappedImage.setDimensions(ENoiseDimension::D4);
	mappedImage.setCoordinates(coordinates);
	auto referenceImage = TSdl<NoiseImage>::make();
	referenceImage.setDimensions(ENoiseDimension::D4);
	const auto mappedTexture = mappedImage.genRealTexture(ctx);
	const auto referenceTexture = referenceImage.genRealTexture(ctx);
	const SampleLocation unrelatedLocation(Vector3R(9.0_r), EColorUsage::Raw);
	const SampleLocation mappedLocation(
		Vector3R(mappedCoordinates.x(), mappedCoordinates.y(), mappedCoordinates.z()),
		EColorUsage::Raw);

	real mappedSample;
	real referenceSample;
	mappedTexture->sample(unrelatedLocation, &mappedSample);
	referenceTexture->sample(mappedLocation, &referenceSample);
	EXPECT_EQ(mappedSample, referenceSample);
}

