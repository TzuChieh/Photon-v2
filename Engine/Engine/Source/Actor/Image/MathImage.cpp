#include "Actor/Image/MathImage.h"
#include "Actor/Basic/exceptions.h"
#include "Core/Texture/constant_textures.h"
#include "Core/Texture/Function/unary_texture_operators.h"
#include "Core/Texture/Function/binary_texture_operators.h"
#include "Core/Texture/Function/ternary_texture_operators.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <memory>
#include <utility>
#include <type_traits>
#include <limits>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(MathImage, Image);

namespace
{

template<typename OperandType, typename OutputType>
inline auto cook_zero_input_operation(
	const EMathImageOp                     operation,
	std::shared_ptr<TTexture<OperandType>> operand)
-> std::shared_ptr<TTexture<OutputType>>
{
	if(!operand)
	{
		PH_LOG(MathImage, Error,
			"No operand provided for {} operation.", TSdlEnum<EMathImageOp>{}[operation]);
		return nullptr;
	}

	switch(operation)
	{
	case EMathImageOp::Absolute:
	{
		using AbsFunc = texfunc::TAbsolute<OperandType, OutputType>;
		return std::make_shared<
			TUnaryTextureOperator<OperandType, OutputType, AbsFunc>>(
				operand);
	}

	default:
		throw CookException("Specified math image operation is not supported.");
	}
}

template<typename OperandType, typename InputType, typename OutputType, typename InputScalarType = real>
inline auto cook_one_input_operation(
	const EMathImageOp                     operation,
	std::shared_ptr<TTexture<OperandType>> operand,
	std::shared_ptr<TTexture<InputType>>   input,
	std::optional<InputScalarType>         inputScalar = std::nullopt)
-> std::shared_ptr<TTexture<OutputType>>
{
	if(!operand)
	{
		PH_LOG(MathImage, Error,
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
				PH_LOG(MathImage, Warning,
					"No value provided for Add operation. Using 0.");
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
				PH_LOG(MathImage, Warning,
					"No value provided for Subtract operation. Using 0.");
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
				PH_LOG(MathImage, Warning,
					"No value provided for Multiply operation. Using 1.");
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
				PH_LOG(MathImage, Warning,
					"No divisor provided for Divide operation. Using 1.");
				inputScalar = InputScalarType{1};
			}

			const InputScalarType divisor = *inputScalar;
			if(divisor == 0)
			{
				PH_LOG(MathImage, Warning, "Division by 0 detected.");
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
				PH_LOG(MathImage, Warning,
					"No exponent provided for Power operation. Using 1.");
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

template
<
	typename OperandType, 
	typename InputType1,
	typename InputType2,
	typename OutputType, 
	typename InputScalarType1 = real,
	typename InputScalarType2 = real
>
inline auto cook_two_inputs_operation(
	const EMathImageOp                     operation,
	std::shared_ptr<TTexture<OperandType>> operand,
	std::shared_ptr<TTexture<InputType1>>  input1,
	std::shared_ptr<TTexture<InputType2>>  input2,
	std::optional<InputScalarType1>        inputScalar1 = std::nullopt,
	std::optional<InputScalarType2>        inputScalar2 = std::nullopt)
-> std::shared_ptr<TTexture<OutputType>>
{
	if(!operand)
	{
		PH_LOG(MathImage, Error,
			"No operand provided for {} operation.", TSdlEnum<EMathImageOp>{}[operation]);
		return nullptr;
	}

	switch(operation)
	{
	case EMathImageOp::Clamp:
	{
		// Only one of the bounds is texture
		if((input1 && !input2) || (!input1 && input2))
		{
			// Always promote the scalar to texture. This can be a pessimization as the types might
			// be able to perform a clamp directly. Detect those types if proves to be problematic.

			if(!input1)
			{
				if(!inputScalar1)
				{
					constexpr auto minValue = std::numeric_limits<InputScalarType1>::lowest();
					PH_LOG(MathImage, Warning,
						"No lower-bound provided for Clamp operation. Using {}.", minValue);
					inputScalar1 = minValue;
				}

				input1 = std::make_shared<TConstantTexture<InputType1>>(InputType1(*inputScalar1));
			}

			if(!input2)
			{
				if(!inputScalar2)
				{
					constexpr auto maxValue = std::numeric_limits<InputScalarType2>::max();
					PH_LOG(MathImage, Warning,
						"No upper-bound provided for Clamp operation. Using {}.", maxValue);
					inputScalar2 = maxValue;
				}

				input2 = std::make_shared<TConstantTexture<InputType2>>(InputType2(*inputScalar2));
			}

			PH_ASSERT(input1 && input2);
		}

		// Both bounds are textures
		if(input1 && input2)
		{
			using ClampFunc = texfunc::TClamp<OperandType, InputType1, InputType2, OutputType>;
			return std::make_shared<
				TTernaryTextureOperator<OperandType, InputType1, InputType2, OutputType, ClampFunc>>(
					operand, input1, input2);
		}
		else
		{
			// We already convert all one-texture cases to two-texture case
			PH_ASSERT(!input1 && !input2);

			const InputScalarType1 lowerBound = *inputScalar1;
			const InputScalarType2 upperBound = *inputScalar2;

			using ClampFunc = texfunc::TClampConstant<OperandType, InputScalarType1, InputScalarType2, OutputType>;
			return std::make_shared<
				TUnaryTextureOperator<OperandType, OutputType, ClampFunc>>(
					operand, ClampFunc(lowerBound, upperBound));
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

	switch(m_mathOp)
	{
	case EMathImageOp::Absolute:
		return cook_zero_input_operation<Image::ArrayType, Image::ArrayType>(
			m_mathOp,
			m_operandImage->genNumericTexture(ctx));

	case EMathImageOp::Clamp:
		return cook_two_inputs_operation<Image::ArrayType, Image::ArrayType, Image::ArrayType, Image::ArrayType, float64, float64>(
			m_mathOp,
			m_operandImage->genNumericTexture(ctx),
			m_imageInput0 ? m_imageInput0->genNumericTexture(ctx) : nullptr,
			m_imageInput1 ? m_imageInput1->genNumericTexture(ctx) : nullptr,
			m_scalarInput0,
			m_scalarInput1);

	default:
		return cook_one_input_operation<Image::ArrayType, Image::ArrayType, Image::ArrayType, float64>(
			m_mathOp, 
			m_operandImage->genNumericTexture(ctx),
			m_imageInput0 ? m_imageInput0->genNumericTexture(ctx) : nullptr,
			m_scalarInput0);
	}
}

std::shared_ptr<TTexture<math::Spectrum>> MathImage::genColorTexture(
	const CookingContext& ctx)
{
	if(!m_operandImage)
	{
		throw CookException("No operand image provided for color texture.");
	}

	switch(m_mathOp)
	{
	case EMathImageOp::Absolute:
		return cook_zero_input_operation<math::Spectrum, math::Spectrum>(
			m_mathOp,
			m_operandImage->genColorTexture(ctx));

	case EMathImageOp::Clamp:
		return cook_two_inputs_operation<math::Spectrum, math::Spectrum, math::Spectrum, math::Spectrum, math::ColorValue, math::ColorValue>(
			m_mathOp,
			m_operandImage->genColorTexture(ctx),
			m_imageInput0 ? m_imageInput0->genColorTexture(ctx) : nullptr,
			m_imageInput1 ? m_imageInput1->genColorTexture(ctx) : nullptr,
			static_cast<math::ColorValue>(m_scalarInput0),
			static_cast<math::ColorValue>(m_scalarInput1));

	default:
		return cook_one_input_operation<math::Spectrum, math::Spectrum, math::Spectrum, math::ColorValue>(
			m_mathOp,
			m_operandImage->genColorTexture(ctx),
			m_imageInput0 ? m_imageInput0->genColorTexture(ctx) : nullptr,
			static_cast<math::ColorValue>(m_scalarInput0));
	}
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
