#pragma once

#include "Actor/Image/Image.h"
#include "Core/Texture/Function/TConstantMultiplyTexture.h"
#include "Core/Texture/Function/TConstantAddTexture.h"

#include <vector>
#include <iostream>
#include <utility>

namespace ph
{

class RealMathImage : public Image, public TCommandInterface<RealMathImage>
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
		CookingContext& context) const override;

	std::shared_ptr<TTexture<math::Vector3R>> genTextureVector3R(
		CookingContext& context) const override;

	std::shared_ptr<TTexture<Spectrum>> genTextureSpectral(
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

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  image       </category>
	<type_name> real-math   </type_name>
	<extend>    image.image </extend>

	<name> Real Math Image </name>
	<description>
		This image applies mathematical modifications on other images.
	</description>

	<command type="creator">
		<input name="math-op" type="string">
			<description>
				The mathematical operation used. "multiply": multiplying a value to the target; 
				"add": add a value to the target.
			</description>
		</input>
		<input name="value" type="real">
			<description>
				The value that is going to be applied to the target. How it will be applied
				depends on the math-op specified.
			</description>
		</input>
		<input name="operand" type="image">
			<description>
				The target image that is going to be operated on.
			</description>
		</input>
	</command>

	</SDL_interface>
*/
