#include <Core/Texture/Function/unary_texture_operators.h>
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

	using Multiplier = texfunc::TMultiplyConstant<real, int8, real>;

	TUnaryTextureOperator<real, real, Multiplier> texture(
		std::make_shared<UnitTexture>(),
		Multiplier(static_cast<int8>(2)));

	real sampleValue;
	texture.sample(SampleLocation(HitDetail()), &sampleValue);
	EXPECT_DOUBLE_EQ(sampleValue, 2.0_r);
}
