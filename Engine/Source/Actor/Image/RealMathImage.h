#pragma once

#include "Actor/Image/Image.h"
#include "Core/Texture/Function/TMultiplyTexture.h"

#include <vector>
#include <iostream>

namespace ph
{

class RealMathImage final : public Image, public TCommandInterface<RealMathImage>
{
public:
	enum class EMathOp
	{
		MULTIPLY
	};

	RealMathImage();
	RealMathImage(EMathOp mathOp, real value);
	virtual ~RealMathImage() override = default;

	virtual std::shared_ptr<TTexture<real>> genTextureReal(
		CookingContext& context) const override;

	virtual std::shared_ptr<TTexture<Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	virtual std::shared_ptr<TTexture<SpectralStrength>> genTextureSpectral(
		CookingContext& context) const override;

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
		const std::shared_ptr<TTexture<InputType>>& operandTexture) const
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
			auto texture = std::make_shared<TMultiplyTexture<InputType, real, OutputType>>();
			texture->setMultiplier(m_real);
			texture->setInputTexture(operandTexture);
			result = texture;
			break;
		}

		default:
			std::cerr << "warning: at ConstantMathImage::genTexture(), "
			          << "unsupported math operation detected" << std::endl;
		}
		return result;
	}

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph