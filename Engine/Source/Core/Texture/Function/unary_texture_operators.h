#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Utility/traits.h"
#include "Math/Color/Spectrum.h"
#include "Math/TArithmeticArray.h"

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

template<typename InputType, typename OutputType>
class TDefaultConversion final
{
public:
	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(IsBuildable<OutputType, InputType>(),
			"<OutputType> must be buildable from <InputType>");

		return OutputType(inputValue);
	}
};

template<typename T, std::size_t N>
class TArrayToSpectrum final
{
public:
	math::Spectrum operator () (const std::array<T, N>& inputValues) const
	{
		if constexpr(N == 1)
		{
			return math::Spectrum(inputValues[0]);
		}
		else
		{
			static_assert(N == math::Spectrum::NUM_VALUES,
				"Cannot convert mismatched number of components from std::array to Spectrum");

			return math::Spectrum(inputValues);
		}
	}
};

template<typename InputType, typename ConstantType, typename OutputType>
class TAddConstant final
{
public:
	explicit TAddConstant(ConstantType constant) :
		m_constant(std::move(constant))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(CCanAdd<InputType, ConstantType, OutputType>,
			"Must have add operator for <OutputType> = <InputType> + <ConstantType>");

		return inputValue + m_constant;
	}

private:
	ConstantType m_constant;
};

template<typename InputType, typename ConstantType, typename OutputType>
class TMultiplyConstant final
{
public:
	explicit TMultiplyConstant(ConstantType constant) :
		m_constant(std::move(constant))
	{}

	OutputType operator () (const InputType& inputValue) const
	{
		static_assert(CCanMultiply<InputType, ConstantType, OutputType>,
			"Must have multiply operator for <OutputType> = <InputType> * <ConstantType>");

		return inputValue * m_constant;
	}

private:
	ConstantType m_constant;
};

template<typename T, std::size_t N>
class TArrayAddScalar final
{
public:
	explicit TArrayAddScalar(T scalar) :
		m_scalar(std::move(scalar))
	{}

	std::array<T, N> operator () (const std::array<T, N>& inputValue) const
	{
		using ComputeType = math::TArithmeticArray<T, N>;
		using AddFunc     = TAddConstant<ComputeType, T, ComputeType>;

		return AddFunc(m_scalar)(ComputeType(inputValue)).toArray();
	}

private:
	T m_scalar;
};

template<typename T, std::size_t N>
class TArrayMultiplyScalar final
{
public:
	explicit TArrayMultiplyScalar(T scalar) :
		m_scalar(std::move(scalar))
	{}

	std::array<T, N> operator () (const std::array<T, N>& inputValue) const
	{
		using ComputeType = math::TArithmeticArray<T, N>;
		using MulFunc     = TMultiplyConstant<ComputeType, T, ComputeType>;

		return MulFunc(m_scalar)(ComputeType(inputValue)).toArray();
	}

private:
	T m_scalar;
};

using SpectrumAddScalar = TAddConstant<math::Spectrum, math::ColorValue, math::Spectrum>;
using SpectrumMultiplyScalar = TMultiplyConstant<math::Spectrum, math::ColorValue, math::Spectrum>;

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
