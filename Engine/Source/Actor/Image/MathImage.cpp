#include "Actor/Image/MathImage.h"
#include "Common/assertion.h"

#include <memory>
#include <utility>

namespace ph
{

MathImage::MathImage() :

	Image(),

	m_mathOp       (EMathImageOp::Add), 
	m_operandImage (nullptr), 
	m_scalarInput  (),
	m_imageInput0  (nullptr),
	m_imageInput1  (nullptr)
{}

std::shared_ptr<TTexture<Image::NumericArray>> MathImage::genNumericTexture(
	ActorCookingContext& ctx)
{

}

std::shared_ptr<TTexture<math::Spectrum>> MathImage::genColorTexture(
	ActorCookingContext& ctx)
{

}

std::shared_ptr<TTexture<real>> RealMathImage::genTextureReal(
	ActorCookingContext& ctx) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureReal(ctx);
	return genTexture<real, real>(std::move(operandTexture));
}

std::shared_ptr<TTexture<math::Vector3R>> RealMathImage::genTextureVector3R(
	ActorCookingContext& ctx) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureVector3R(ctx);
	return genTexture<math::Vector3R, math::Vector3R>(std::move(operandTexture));
}

std::shared_ptr<TTexture<math::Spectrum>> RealMathImage::genTextureSpectral(
	ActorCookingContext& ctx) const
{
	auto operandImage = checkOperandImage();
	if(!operandImage)
	{
		return nullptr;
	}

	auto operandTexture = operandImage->genTextureSpectral(ctx);
	return genTexture<math::Spectrum, math::Spectrum>(std::move(operandTexture));
}

MathImage& MathImage::setOperation(const EMathImageOp op)
{
	m_mathOp = op;
	return *this;
}

MathImage& MathImage::setOperandImage(std::shared_ptr<Image> operand)
{
	m_operandImage = std::move(operand);
	return *this;
}

MathImage& MathImage::setScalarInput(const float64 value)
{
	m_scalarInput = value;
	return *this;
}

MathImage& MathImage::setInputImage0(std::shared_ptr<Image> input)
{
	m_imageInput0 = std::move(input);
	return *this;
}

MathImage& MathImage::setInputImage1(std::shared_ptr<Image> input)
{
	m_imageInput1 = std::move(input);
	return *this;
}

}// end namespace ph
