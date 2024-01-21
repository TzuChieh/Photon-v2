#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/SampleLocation.h"
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
#include <cmath>

namespace ph
{

namespace texfunc
{

template<typename OperatorType, typename InputTypeA, typename InputTypeB, typename OutputType>
concept CBinaryOperator = requires (OperatorType op, InputTypeA inputA, InputTypeB inputB)
{
	{ op(inputA, inputB) } -> std::same_as<OutputType>;
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TAdd final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		static_assert(CCanAdd<InputTypeA, InputTypeB, OutputType>,
			"Must have addition operator for `OutputType` = `InputTypeA` + `InputTypeB`");

		return inputValueA + inputValueB;
	}
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TSubtract final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		static_assert(CCanSubtract<InputTypeA, InputTypeB, OutputType>,
			"Must have subtraction operator for `OutputType` = `InputTypeA` - `InputTypeB`");

		return inputValueA - inputValueB;
	}
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TMultiply final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		static_assert(CCanMultiply<InputTypeA, InputTypeB, OutputType>,
			"Must have multiplication operator for `OutputType` = `InputTypeA` * `InputTypeB`");

		return inputValueA * inputValueB;
	}
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TDivide final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		static_assert(CCanDivide<InputTypeA, InputTypeB, OutputType>,
			"Must have multiplication operator for `OutputType` = `InputTypeA` / `InputTypeB`");

		return inputValueA / inputValueB;
	}
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TPower final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		constexpr bool canCallPowMethod = requires (InputTypeA a, InputTypeB b)
		{
			{ a.pow(b) } -> std::convertible_to<OutputType>;
		};

		constexpr bool canCallStdPow = requires (InputTypeA a, InputTypeB b)
		{
			{ std::pow(a, b) } -> std::convertible_to<OutputType>;
		};

		if constexpr(canCallPowMethod)
		{
			return inputValueA.pow(inputValueB);
		}
		else if constexpr(canCallStdPow)
		{
			return std::pow(inputValueA, inputValueB);
		}
		else
		{
			PH_STATIC_ASSERT_DEPENDENT_FALSE(OutputType,
				"Cannot perform power operation for the specified types.");
		}
	}
};

using AddSpectrum      = TAdd<math::Spectrum, math::Spectrum, math::Spectrum>;
using SubtractSpectrum = TSubtract<math::Spectrum, math::Spectrum, math::Spectrum>;
using MultiplySpectrum = TMultiply<math::Spectrum, math::Spectrum, math::Spectrum>;
using DivideSpectrum   = TDivide<math::Spectrum, math::Spectrum, math::Spectrum>;
using PowerSpectrum    = TPower<math::Spectrum, math::Spectrum, math::Spectrum>;

}// end namespace texfunc

template
<
	typename InputTypeA, 
	typename InputTypeB, 
	typename OutputType,
	texfunc::CBinaryOperator<InputTypeA, InputTypeB, OutputType> OperatorType
>
class TBinaryTextureOperator : public TTexture<OutputType>
{
public:
	using InputTexResA = std::shared_ptr<TTexture<InputTypeA>>;
	using InputTexResB = std::shared_ptr<TTexture<InputTypeB>>;

	TBinaryTextureOperator(InputTexResA inputA, InputTexResB inputB) requires std::default_initializable<OperatorType>
		: TBinaryTextureOperator(std::move(inputA), std::move(inputB), OperatorType{})
	{}

	TBinaryTextureOperator(InputTexResA inputA, InputTexResB inputB, OperatorType op)
		: m_inputA  (std::move(inputA))
		, m_inputB  (std::move(inputB))
		, m_operator(std::move(op))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(m_inputA);
		PH_ASSERT(m_inputB);
		PH_ASSERT(out_value);

		InputTypeA inputValueA;
		m_inputA->sample(sampleLocation, &inputValueA);

		InputTypeB inputValueB;
		m_inputB->sample(sampleLocation, &inputValueB);

		*out_value = m_operator(inputValueA, inputValueB);
	}

private:
	InputTexResA m_inputA;
	InputTexResB m_inputB;
	OperatorType m_operator;
};

}// end namespace ph
