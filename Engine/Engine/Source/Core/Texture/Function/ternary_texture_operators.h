#pragma once

#include "Core/Texture/TTexture.h"
#include "Core/Texture/SampleLocation.h"
#include "Utility/traits.h"
#include "Math/math.h"
#include "Math/Color/Spectrum.h"
#include "Math/TArithmeticArray.h"

#include <Common/assertion.h>

#include <type_traits>
#include <memory>
#include <utility>
#include <concepts>
#include <cmath>

namespace ph
{

namespace texfunc
{

template<typename OperatorType, typename InputTypeA, typename InputTypeB, typename InputTypeC, typename OutputType>
concept CTernaryOperator = requires (OperatorType op, InputTypeA inputA, InputTypeB inputB, InputTypeC inputC)
{
	{ op(inputA, inputB, inputC) } -> std::same_as<OutputType>;
};

template<typename InputTypeA, typename InputTypeB, typename InputTypeC, typename OutputType>
class TClamp final
{
public:
	OutputType operator () (
		const InputTypeA& inputValueA, 
		const InputTypeB& inputValueB,
		const InputTypeC& inputValueC) const
	{
		constexpr bool canCallClampMethod = requires (InputTypeA a, InputTypeB b, InputTypeC c)
		{
			{ a.clamp(b, c) } -> std::convertible_to<OutputType>;
		};

		constexpr bool canCallMathClamp = requires (InputTypeA a, InputTypeB b, InputTypeC c)
		{
			{ math::clamp(a, b, c) } -> std::convertible_to<OutputType>;
		};

		if constexpr(canCallClampMethod)
		{
			return inputValueA.clamp(inputValueB, inputValueC);
		}
		else if constexpr(canCallMathClamp)
		{
			return math::clamp(inputValueA, inputValueB, inputValueC);
		}
		else
		{
			PH_STATIC_ASSERT_DEPENDENT_FALSE(OutputType,
				"Cannot perform clamp operation for the specified types.");
		}
	}
};

using ClampSpectrum = TClamp<math::Spectrum, math::Spectrum, math::Spectrum, math::Spectrum>;

}// end namespace texfunc

template
<
	typename InputTypeA, 
	typename InputTypeB,
	typename InputTypeC,
	typename OutputType,
	texfunc::CTernaryOperator<InputTypeA, InputTypeB, InputTypeC, OutputType> OperatorType
>
class TTernaryTextureOperator : public TTexture<OutputType>
{
public:
	using InputTexResA = std::shared_ptr<TTexture<InputTypeA>>;
	using InputTexResB = std::shared_ptr<TTexture<InputTypeB>>;
	using InputTexResC = std::shared_ptr<TTexture<InputTypeC>>;

	TTernaryTextureOperator(
		InputTexResA inputA, 
		InputTexResB inputB, 
		InputTexResC inputC) requires std::default_initializable<OperatorType>

		: TTernaryTextureOperator(
			std::move(inputA), 
			std::move(inputB), 
			std::move(inputC), 
			OperatorType{})
	{}

	TTernaryTextureOperator(
		InputTexResA inputA, 
		InputTexResB inputB,
		InputTexResC inputC,
		OperatorType op)

		: m_inputA  (std::move(inputA))
		, m_inputB  (std::move(inputB))
		, m_inputC  (std::move(inputC))
		, m_operator(std::move(op))
	{}

	void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(m_inputA);
		PH_ASSERT(m_inputB);
		PH_ASSERT(m_inputC);
		PH_ASSERT(out_value);

		InputTypeA inputValueA;
		m_inputA->sample(sampleLocation, &inputValueA);

		InputTypeB inputValueB;
		m_inputB->sample(sampleLocation, &inputValueB);

		InputTypeC inputValueC;
		m_inputC->sample(sampleLocation, &inputValueC);

		*out_value = m_operator(inputValueA, inputValueB, inputValueC);
	}

private:
	InputTexResA m_inputA;
	InputTexResB m_inputB;
	InputTexResC m_inputC;
	OperatorType m_operator;
};

}// end namespace ph
