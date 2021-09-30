#include "Actor/Image/RealMathImage.h"
#include "Common/assertion.h"

#include <memory>
#include <utility>

namespace ph
{

RealMathImage::RealMathImage() :
	RealMathImage(EMathOp::MULTIPLY, 1.0_r)
{}

RealMathImage::RealMathImage(const EMathOp mathOp, const real value) :
	Image(),
	m_mathOp(mathOp), m_real(value), m_operandImage()
{}

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

RealMathImage& RealMathImage::setOperandImage(const std::shared_ptr<Image>& operand)
{
	PH_ASSERT(operand);
	m_operandImage = operand;

	return *this;
}

RealMathImage& RealMathImage::setMathOp(const EMathOp mathOp)
{
	m_mathOp = mathOp;

	return *this;
}

RealMathImage& RealMathImage::setReal(const real value)
{
	m_real = value;

	return *this;
}

std::shared_ptr<Image> RealMathImage::checkOperandImage() const
{
	const auto operandImage = m_operandImage.lock();
	if(!operandImage)
	{
		std::cout << "note: at ConstantMathImage::checkOperandImage(), "
		          << "parent image is not set" << std::endl;
	}
	return operandImage;
}

}// end namespace ph
