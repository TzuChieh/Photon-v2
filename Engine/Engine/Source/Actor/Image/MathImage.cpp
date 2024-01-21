#include "Actor/Image/MathImage.h"
#include "Actor/Basic/exceptions.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Core/Texture/Function/binary_texture_operators.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(MathImage, Image);

namespace
{

template<typename OperandType, typename InputType, typename OutputType, typename InputScalarType = real>
inline auto cook_single_input_operation(
	const EMathImageOp                            operation,
	const std::shared_ptr<TTexture<OperandType>>& operand,
	const std::shared_ptr<TTexture<InputType>>&   input,
	std::optional<InputScalarType>                inputScalar = std::nullopt)
-> std::shared_ptr<TTexture<OutputType>>
{
	if(!operand)
	{
		PH_LOG_ERROR(MathImage,
			"No operand provided for {} operation.", TSdlEnum<EMathImageOp>{}[operation]);
		return nullptr;
	}

	switch(operation)
	{
	case EMathImageOp::Add:
	{
		if(input)
		{
			using AddFunc = texfunc::TAdd<OperandType, InputType, OutputType>;
			return std::make_shared<
				TBinaryTextureOperator<OperandType, InputType, OutputType, AddFunc>>(
					operand, input);
		}
		else
		{
			if(!inputScalar)
			{
				PH_LOG_WARNING(MathImage,
					"No input provided for Add operation. Using 0.");
				inputScalar = InputScalarType{0};
			}

			using AddFunc = texfunc::TAddConstant<OperandType, InputScalarType, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, AddFunc>>(
					operand, AddFunc(*inputScalar));
		}
	}

	case EMathImageOp::Subtract:
	{
		if(input)
		{
			using SubFunc = texfunc::TSubtract<OperandType, InputType, OutputType>;
			return std::make_shared<
				TBinaryTextureOperator<OperandType, InputType, OutputType, SubFunc>>(
					operand, input);
		}
		else
		{
			if(!inputScalar)
			{
				PH_LOG_WARNING(MathImage,
					"No input provided for Subtract operation. Using 0.");
				inputScalar = InputScalarType{0};
			}

			using SubFunc = texfunc::TSubtractConstant<OperandType, InputScalarType, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, SubFunc>>(
					operand, SubFunc(*inputScalar));
		}
	}

	case EMathImageOp::Multiply:
	{
		if(input)
		{
			using MulFunc = texfunc::TMultiply<OperandType, InputType, OutputType>;
			return std::make_shared<
				TBinaryTextureOperator<OperandType, InputType, OutputType, MulFunc>>(
					operand, input);
		}
		else
		{
			if(!inputScalar)
			{
				PH_LOG_WARNING(MathImage,
					"No input provided for Multiply operation. Using 1.");
				inputScalar = InputScalarType{1};
			}

			using MulFunc = texfunc::TMultiplyConstant<OperandType, InputScalarType, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, MulFunc>>(
					operand, MulFunc(*inputScalar));
		}
	}

	case EMathImageOp::Divide:
	{
		if(input)
		{
			using DivFunc = texfunc::TDivide<OperandType, InputType, OutputType>;
			return std::make_shared<
				TBinaryTextureOperator<OperandType, InputType, OutputType, DivFunc>>(
					operand, input);
		}
		else
		{
			if(!inputScalar)
			{
				PH_LOG_WARNING(MathImage,
					"No input provided for Divide operation. Using 1.");
				inputScalar = InputScalarType{1};
			}

			const InputScalarType divisor = *inputScalar;
			if(divisor == 0)
			{
				PH_LOG_WARNING(MathImage, "Division by 0 detected.");
			}

			using DivFunc = texfunc::TDivideConstant<OperandType, InputScalarType, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, DivFunc>>(
					operand, DivFunc(divisor));
		}
	}

	case EMathImageOp::Power:
	{
		if(input)
		{
			using PowFunc = texfunc::TPower<OperandType, InputType, OutputType>;
			return std::make_shared<
				TBinaryTextureOperator<OperandType, InputType, OutputType, PowFunc>>(
					operand, input);
		}
		else
		{
			if(!inputScalar)
			{
				PH_LOG_WARNING(MathImage,
					"No input provided for Power operation. Using 1.");
				inputScalar = InputScalarType{1};
			}

			const InputScalarType exponent = *inputScalar;

			using PowFunc = texfunc::TPowerConstant<OperandType, InputScalarType, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, PowFunc>>(
					operand, PowFunc(exponent));
		}
	}

	default:
		throw CookException("Specified math image operation is not supported.");
	}
}

}// end anonymous namespace

MathImage::MathImage()

	: Image()

	, m_mathOp      (EMathImageOp::Add)
	, m_operandImage(nullptr)
	, m_scalarInput0(0.0)
	, m_scalarInput1(0.0)
	, m_imageInput0 (nullptr)
	, m_imageInput1 (nullptr)
{}

std::shared_ptr<TTexture<Image::ArrayType>> MathImage::genNumericTexture(
	const CookingContext& ctx)
{
	if(!m_operandImage)
	{
		throw CookException("No operand image provided for numeric texture.");
	}

	return cook_single_input_operation<Image::ArrayType, Image::ArrayType, Image::ArrayType, float64>(
		m_mathOp, 
		m_operandImage->genNumericTexture(ctx),
		m_imageInput0 ? m_imageInput0->genNumericTexture(ctx) : nullptr,
		m_scalarInput0);
}

std::shared_ptr<TTexture<math::Spectrum>> MathImage::genColorTexture(
	const CookingContext& ctx)
{
	if(!m_operandImage)
	{
		throw CookException("No operand image provided for color texture.");
	}

	return cook_single_input_operation<math::Spectrum, math::Spectrum, math::Spectrum, math::ColorValue>(
		m_mathOp,
		m_operandImage->genColorTexture(ctx),
		m_imageInput0 ? m_imageInput0->genColorTexture(ctx) : nullptr,
		static_cast<math::ColorValue>(m_scalarInput0));
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

MathImage& MathImage::setScalarInput0(const float64 value)
{
	m_scalarInput0 = value;
	return *this;
}

MathImage& MathImage::setScalarInput1(const float64 value)
{
	m_scalarInput1 = value;
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
