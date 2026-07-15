#include <Engine/Actor/Image/ConstantImage.h>
#include <Engine/Actor/Image/SwizzledImage.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Math/Color/color_enums.h>
#include <Engine/World/Foundation/CookedResourceCollection.h>
#include <Engine/World/Foundation/CookingContext.h>

#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace ph;
using namespace ph::math;

TEST(SwizzledImageTest, SelectsRawComponent)
{
	CookedResourceCollection resources;
	CookingContext ctx(&resources, nullptr);

	auto input = std::make_shared<ConstantImage>();
	input->setRaw(std::vector<float64>{0.2, 0.4, 0.6, 0.8});

	SwizzledImage image;
	image.setInput(input).setSwizzleSubscripts("b");
	const auto texture = image.genRealTexture(ctx);

	real sampled;
	texture->sample(SampleLocation(Vector2R(0), EColorUsage::Raw), &sampled);
	EXPECT_EQ(sampled, 0.6_r);
}

