#pragma once

#include "Actor/Image/Image.h"
#include "Core/Texture/Function/TConstantMultiplyTexture.h"
#include "Core/Texture/Function/TConstantAddTexture.h"

#include <vector>
#include <iostream>
#include <utility>

namespace ph
{

class RealMathImage : public Image
{
public:
	enum class EMathOp
	{
		MULTIPLY,
		ADD
	};

	RealMathImage();
	RealMathImage(EMathOp mathOp, real value);

	std::shared_ptr<TTexture<real>> genTextureReal(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		ActorCookingContext& ctx) const override;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
		ActorCookingContext& ctx) const override;

	RealMathImage& setOperandImage(const std::shared_ptr<Image>& operand);
	RealMathImage& setMathOp(EMathOp mathOp);
	RealMathImage& setReal(real value);

private:
	EMathOp              m_mathOp;
	real                 m_real;
	std::weak_ptr<Image> m_operandImage;

	std::shared_ptr<Image> checkOperandImage() const;

	template<typename InputType, typename OutputType>
	std::shared_ptr<TTexture<OutputType>> genTexture(
		std::shared_ptr<TTexture<InputType>> operandTexture) const
	{
		if(!operandTexture)
		{
			std::cerr << "warning: at ConstantMathImage::genTexture(), "
			          << "no operand texture (null texture detected)" << std::endl;
			return nullptr;
		}

		std::shared_ptr<TTexture<OutputType>> result;
		switch(m_mathOp)
		{
		case EMathOp::MULTIPLY:
		{
			result = std::make_shared<
				TConstantMultiplyTexture<InputType, real, OutputType>>(std::move(operandTexture), m_real);
			break;
		}

		case EMathOp::ADD:
		{
			result = std::make_shared<
				TConstantAddTexture<InputType, real, OutputType>>(std::move(operandTexture), m_real);
			break;
		}

		default:
			std::cerr << "warning: at ConstantMathImage::genTexture(), "
			          << "unsupported math operation detected" << std::endl;
			break;
		}
		return result;
	}
};

}// end namespace ph
