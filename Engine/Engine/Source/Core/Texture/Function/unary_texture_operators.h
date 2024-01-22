#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/SampleLocation.h"
#include "Core/Texture/Function/binary_texture_operators.h"
#include "Core/Texture/Function/ternary_texture_operators.h"
#include "Utility/traits.h"
#include "Math/Color/Spectrum.h"
#include "Math/TArithmeticArray.h"

#include <Common/assertion.h>

#include <cstddef>
#include <type_traits>
#include <memory>
#include <utility>
#include <concepts>
#include <array>

namespace ph
{

namespace texfunc
{

template<typename OperatorType, typename InputType, typename OutputType>
concept CUnaryOperator = requires (OperatorType op, InputType input)
{
	{ op(input) } -> std::same_as<OutputType>;
};

/*! @brief Constructs output value from input value.
*/
template<typename InputType, typename OutputType>
class TDefaultConversion final
{
public:
	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(IsBuildable<OutputType, InputType>(),
			"`OutputType` must be buildable from `InputType`");

		return OutputType(inputValue);
	}
};

/*! @brief Converts a scalar value to spectrum.
*/
template<typename T>
class TScalarToSpectrum final
{
public:
	math::Spectrum operator () (const T scalarValue) const
	{
		return math::Spectrum(scalarValue);
	}

	math::Spectrum operator () (const std::array<T, 1>& scalarValue) const
	{
		return (*this)(scalarValue[0]);
	}

	math::Spectrum operator () (const math::TArithmeticArray<T, 1>& scalarValue) const
	{
		return (*this)(scalarValue[0]);
	}
};

template<typename InputType, typename OutputType>
class TAbsolute final
{
public:
	OutputType operator () (const InputType& inputValue) const
	{
		constexpr bool canCallAbsMethod = requires (InputType input)
		{
			{ input.abs() } -> std::convertible_to<OutputType>;
		};

		constexpr bool canCallStdAbs = requires (InputType input)
		{
			{ std::abs(input) } -> std::convertible_to<OutputType>;
		};

		if constexpr(canCallAbsMethod)
		{
			return inputValue.abs();
		}
		else if constexpr(canCallStdAbs)
		{
			return std::abs(inputValue);
		}
		else
		{
			PH_STATIC_ASSERT_DEPENDENT_FALSE(OutputType,
				"Cannot perform absolute operation for the specified types.");
		}
	}
};

/*! @brief Uses binary operator as a unary one by treating the second input as constant.
*/
template
<
	typename InputType, 
	typename ConstantType,
	typename OutputType,
	CBinaryOperator<InputType, ConstantType, OutputType> BinaryOperatorType
>
class TUnaryFromBinary final
{
public:
	explicit TUnaryFromBinary(ConstantType constant)
		: m_constant(std::move(constant))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(std::default_initializable<BinaryOperatorType>);

		return BinaryOperatorType{}(inputValue, m_constant);
	}

private:
	ConstantType m_constant;
};

/*! @brief Uses ternary operator as a unary one by treating the second and third inputs as constants.
*/
template
<
	typename InputType, 
	typename ConstantTypeA,
	typename ConstantTypeB,
	typename OutputType,
	CTernaryOperator<InputType, ConstantTypeA, ConstantTypeB, OutputType> TernaryOperatorType
>
class TUnaryFromTernary final
{
public:
	TUnaryFromTernary(ConstantTypeA constantA, ConstantTypeB constantB)
		: m_constantA(std::move(constantA))
		, m_constantB(std::move(constantB))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(std::default_initializable<TernaryOperatorType>);

		return TernaryOperatorType{}(inputValue, m_constantA, m_constantB);
	}

private:
	ConstantTypeA m_constantA;
	ConstantTypeB m_constantB;
};

template<typename InputType, typename ConstantType, typename OutputType>
using TAddConstant = TUnaryFromBinary<
	InputType, 
	ConstantType, 
	OutputType, 
	TAdd<InputType, ConstantType, OutputType>>;

template<typename InputType, typename ConstantType, typename OutputType>
using TSubtractConstant = TUnaryFromBinary<
	InputType,
	ConstantType,
	OutputType,
	TSubtract<InputType, ConstantType, OutputType>>;

template<typename InputType, typename ConstantType, typename OutputType>
using TMultiplyConstant = TUnaryFromBinary<
	InputType,
	ConstantType,
	OutputType,
	TMultiply<InputType, ConstantType, OutputType>>;

template<typename InputType, typename ConstantType, typename OutputType>
using TDivideConstant = TUnaryFromBinary<
	InputType,
	ConstantType,
	OutputType,
	TDivide<InputType, ConstantType, OutputType>>;

template<typename InputType, typename ConstantType, typename OutputType>
using TPowerConstant = TUnaryFromBinary<
	InputType,
	ConstantType,
	OutputType,
	TPower<InputType, ConstantType, OutputType>>;

template<typename InputType, typename ConstantTypeA, typename ConstantTypeB, typename OutputType>
using TClampConstant = TUnaryFromTernary<
	InputType,
	ConstantTypeA,
	ConstantTypeB,
	OutputType,
	TClamp<InputType, ConstantTypeA, ConstantTypeB, OutputType>>;

using SpectrumAddScalar      = TAddConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
using SpectrumSubtractScalar = TSubtractConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
using SpectrumMultiplyScalar = TMultiplyConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
using SpectrumDivideScalar   = TDivideConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
using SpectrumPowerScalar    = TPowerConstant<math::Spectrum, math::ColorValue, math::Spectrum>;

}// end namespace texfunc

template
<
	typename InputType, 
	typename OutputType, 
	texfunc::CUnaryOperator<InputType, OutputType> OperatorType
>
class TUnaryTextureOperator : public TTexture<OutputType>
{
public:
	using InputTexRes = std::shared_ptr<TTexture<InputType>>;

	explicit TUnaryTextureOperator(InputTexRes inputTexture) requires std::default_initializable<OperatorType> :
		TUnaryTextureOperator(std::move(inputTexture), OperatorType())
	{}

	TUnaryTextureOperator(InputTexRes inputTexture, OperatorType op) :
		m_inputTexture(std::move(inputTexture)),
		m_operator    (std::move(op))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(m_inputTexture);
		PH_ASSERT(out_value);

		InputType inputValue;
		m_inputTexture->sample(sampleLocation, &inputValue);

		*out_value = m_operator(inputValue);
	}

private:
	InputTexRes  m_inputTexture;
	OperatorType m_operator;
};

template<typename InputType, typename OutputType>
using TConversionTexture = TUnaryTextureOperator<InputType, OutputType, texfunc::TDefaultConversion<InputType, OutputType>>;

}// end namespace ph
