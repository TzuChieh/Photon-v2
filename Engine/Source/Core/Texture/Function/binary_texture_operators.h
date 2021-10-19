#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"
#include "Utility/traits.h"
#include "Math/Color/Spectrum.h"

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
		static_assert(CanAdd<InputTypeA, InputTypeB, OutputType>::value,
			"Must have add operator for <OutputType> = <InputTypeA> + <InputTypeB>");

		return inputValueA + inputValueB;
	}
};

template<typename InputTypeA, typename InputTypeB, typename OutputType>
class TMultiply final
{
public:
	OutputType operator () (const InputTypeA& inputValueA, const InputTypeB& inputValueB) const
	{
		static_assert(CanMultiply<InputTypeA, InputTypeB, OutputType>::value,
			"Must have multiply operator for <OutputType> = <InputTypeA> * <InputTypeB>");

		return inputValueA * inputValueB;
	}
};

}// end namespace texfunc

template<typename InputTypeA, typename InputTypeB, typename OutputType, typename OperatorType>
requires texfunc::CBinaryOperator<OperatorType, InputTypeA, InputTypeB, OutputType>
class TBinaryTextureOperator : public TTexture<OutputType>
{
public:
	using InputTexResA = std::shared_ptr<TTexture<InputTypeA>>;
	using InputTexResB = std::shared_ptr<TTexture<InputTypeB>>;

	TBinaryTextureOperator(InputTexResA inputA, InputTexResB inputB) requires std::default_initializable<OperatorType> :
		TBinaryTextureOperator(std::move(inputA), std::move(inputB), OperatorType())
	{}

	TBinaryTextureOperator(InputTexResA inputA, InputTexResB inputB, OperatorType op) :
		m_inputA  (std::move(inputA)),
		m_inputB  (std::move(inputB)),
		m_operator(std::move(op))
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
