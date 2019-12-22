#include <Core/Texture/Function/TConstantMultiplyTexture.h>
#include <Core/Texture/TTexture.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TConstantMultiplyTextureTest, CorrectlyMultiply)
{
	class UnitTexture : public TTexture<real>
	{
	public:
		inline virtual ~UnitTexture() override = default;
		inline virtual void sample(const SampleLocation& sampleLocation, real* out_value) const override
		{
			*out_value = 1.0_r;
		}
	};

	TConstantMultiplyTexture<real, int8, real> texture(
		std::make_shared<UnitTexture>(),
		static_cast<int8>(2));

	real sampleValue;
	texture.sample(SampleLocation(HitDetail()), &sampleValue);
	EXPECT_DOUBLE_EQ(sampleValue, 2.0_r);
}
