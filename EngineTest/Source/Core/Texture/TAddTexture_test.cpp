#include <Core/Texture/Function/TAddTexture.h>
#include <Core/Texture/TTexture.h>

#include <gtest/gtest.h>

TEST(TAddTextureTest, CorrectlyAdd)
{
	using namespace ph;

	class MockTexture : public TTexture<real>
	{
	public:
		inline virtual ~MockTexture() override = default;
		inline virtual void sample(const SampleLocation& sampleLocation, real* out_value) const override
		{
			*out_value = 1.0_r;
		}
	};

	auto mockTexture = std::make_shared<MockTexture>();
	TAddTexture<real, int8, real> texture(static_cast<int8>(2));
	texture.setInputTexture(mockTexture);

	real sampleValue;
	texture.sample(SampleLocation(HitDetail()), &sampleValue);
	EXPECT_DOUBLE_EQ(sampleValue, 3.0_r);
}