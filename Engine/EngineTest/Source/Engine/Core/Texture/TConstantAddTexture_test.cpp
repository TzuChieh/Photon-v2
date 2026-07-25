#include <Engine/Core/Texture/Function/unary_texture_operators.h>
#include <Engine/Core/Texture/SampleLocation.h>
#include <Engine/Core/Texture/TTexture.h>

#include <gtest/gtest.h>

TEST(TConstantAddTextureTest, CorrectlyAdd)
{
	using namespace ph;

	class UnitTexture : public TTexture<real>
	{
	public:
		inline virtual ~UnitTexture() override = default;
		inline virtual void sample(const SampleLocation& sampleLocation, real* out_value) const override
		{
			*out_value = 1.0_r;
		}
	};

	using Adder = texfunc::TAddConstant<real, int8, real>;

	TUnaryTextureOperator<real, real, Adder> texture(
		std::make_shared<UnitTexture>(),
		Adder(static_cast<int8>(2)));

	real sampleValue;
	texture.sample(SampleLocation(math::Vector3R(0)), &sampleValue);
	EXPECT_DOUBLE_EQ(sampleValue, 3.0_r);
}
