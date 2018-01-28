#include <Core/Texture/Function/TMultiplyTexture.h>
#include <Core/Texture/TTexture.h>

#include <gtest/gtest.h>

TEST(TMultiplyTextureTest, CorrectlyMultiply)
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
	TMultiplyTexture<real, int8, real> texture(static_cast<int8>(2));
	texture.setInputTexture(mockTexture);

	real sampleValue;
	texture.sample(SampleLocation(HitDetail(), Vector3R()), &sampleValue);
	EXPECT_FLOAT_EQ(sampleValue, 2.0_r);
}