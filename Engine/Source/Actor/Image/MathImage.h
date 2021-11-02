#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

#include <vector>
#include <utility>
#include <optional>

namespace ph
{

enum class EMathImageOp
{
	Add = 0,
	Multiply
};

class MathImage : public Image
{
public:
	MathImage();

	std::shared_ptr<TTexture<Image::NumericArray>> genNumericTexture(
		ActorCookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		ActorCookingContext& ctx) override;

	MathImage& setOperation(EMathImageOp op);
	MathImage& setOperandImage(std::shared_ptr<Image> operand);
	MathImage& setScalarInput(float64 value);
	MathImage& setInputImage0(std::shared_ptr<Image> input);
	MathImage& setInputImage1(std::shared_ptr<Image> input);

private:
	EMathImageOp           m_mathOp;
	std::shared_ptr<Image> m_operandImage;
	float64                m_scalarInput;
	std::shared_ptr<Image> m_imageInput0;
	std::shared_ptr<Image> m_imageInput1;

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
