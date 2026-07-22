#include <Engine/Actor/Basic/TransformInfo.h>
#include <Engine/Actor/Image/CheckerboardImage.h>
#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Image/TransformedImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/Math/TVector3.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <memory>

using namespace ph;
using namespace ph::math;

TEST(TransformedImageTest, ScalesUvw)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto oddImage = std::make_shared<ConstantImage>();
	oddImage->setRaw(1.0_r);
	auto evenImage = std::make_shared<ConstantImage>();
	evenImage->setRaw(0.0_r);

	auto checkerboard = std::make_shared<CheckerboardImage>();
	checkerboard->setNumTiles(2.0_r, 2.0_r);
	checkerboard->setOddImage(oddImage);
	checkerboard->setEvenImage(evenImage);

	auto transform = TSdl<TransformInfo>::make();
	transform.setScale(4.0_r);

	TransformedImage image;
	image.setInput(checkerboard).setTransform(transform);

	const auto texture = image.genRealTexture(ctx);

	real sampled;
	texture->sample(
		SampleLocation(Vector3R(0.2_r, 0.1_r, 0.0_r), EColorUsage::Raw),
		&sampled);

	// Scaling maps (0.2, 0.1) to (0.8, 0.4), changing the 2x2 checker from even to odd
	EXPECT_EQ(sampled, 1.0_r);
}
