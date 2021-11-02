#include "Actor/Image/MathImage.h"
#include "Common/assertion.h"
#include "Common/logging.h"
#include "Actor/actor_exceptions.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Core/Texture/Function/binary_texture_operators.h"

#include <memory>
#include <utility>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(MathImage, Image);

MathImage::MathImage() :

	Image(),

	m_mathOp       (EMathImageOp::Add), 
	m_operandImage (nullptr), 
	m_scalarInput  (0.0),
	m_imageInput0  (nullptr),
	m_imageInput1  (nullptr)
{}

std::shared_ptr<TTexture<Image::NumericArray>> MathImage::genNumericTexture(
	ActorCookingContext& ctx)
{
	if(!m_operandImage)
	{
		throw ActorCookException("No operand image provided for numeric texture.");
	}

	auto operandTexture = m_operandImage->genNumericTexture(ctx);

	switch(m_mathOp)
	{
	case EMathImageOp::Add:
	{
		if(m_imageInput0)
		{
			using AddFunc = texfunc::TAddArray<float64, Image::NUMERIC_ARRAY_SIZE>;
			return std::make_shared<TBinaryTextureOperator<Image::NumericArray, Image::NumericArray, Image::NumericArray, AddFunc>>(
				operandTexture, m_imageInput0->genNumericTexture(ctx));
		}
		else
		{
			// TODO
			using AddFunc = texfunc::TAddConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
			return std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, AddFunc>>(
				operandTexture, AddFunc(static_cast<math::ColorValue>(m_scalarInput)));
		}
	}

	case EMathImageOp::Multiply:
	{
		if(m_imageInput0)
		{
			using MulFunc = texfunc::TMultiplyArray<float64, Image::NUMERIC_ARRAY_SIZE>;
			return std::make_shared<TBinaryTextureOperator<Image::NumericArray, Image::NumericArray, Image::NumericArray, MulFunc>>(
				operandTexture, m_imageInput0->genNumericTexture(ctx));
		}
		else
		{
			using MulFunc = texfunc::TMultiplyConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
			return std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, MulFunc>>(
				operandTexture, MulFunc(static_cast<math::ColorValue>(m_scalarInput)));
		}
	}

	default:
		throw ActorCookException("Specified math image operation is not supported on numeric texture.");
	}
}

std::shared_ptr<TTexture<math::Spectrum>> MathImage::genColorTexture(
	ActorCookingContext& ctx)
{
	if(!m_operandImage)
	{
		throw ActorCookException("No operand image provided for color texture.");
	}

	auto operandTexture = m_operandImage->genColorTexture(ctx);

	switch(m_mathOp)
	{
	case EMathImageOp::Add:
	{
		if(m_imageInput0)
		{
			return std::make_shared<TBinaryTextureOperator<math::Spectrum, math::Spectrum, math::Spectrum, texfunc::AddSpectrum>>(
				operandTexture, m_imageInput0->genColorTexture(ctx));
		}
		else
		{
			using AddFunc = texfunc::TAddConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
			return std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, AddFunc>>(
				operandTexture, AddFunc(static_cast<math::ColorValue>(m_scalarInput)));
		}
	}

	case EMathImageOp::Multiply:
	{
		if(m_imageInput0)
		{
			return std::make_shared<TBinaryTextureOperator<math::Spectrum, math::Spectrum, math::Spectrum, texfunc::MultiplySpectrum>>(
				operandTexture, m_imageInput0->genColorTexture(ctx));
		}
		else
		{
			using MulFunc = texfunc::TMultiplyConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
			return std::make_shared<TUnaryTextureOperator<math::Spectrum, math::Spectrum, MulFunc>>(
				operandTexture, MulFunc(static_cast<math::ColorValue>(m_scalarInput)));
		}
	}

	default:
		throw ActorCookException("Specified math image operation is not supported on color texture.");
	}
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
