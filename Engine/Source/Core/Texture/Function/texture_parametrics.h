#pragma once

#include "Core/Texture/TTexture.h"
#include "Common/assertion.h"
#include "Core/Texture/SampleLocation.h"

#include <memory>
#include <utility>
#include <concepts>

namespace ph
{

namespace texfunc
{



}// end namespace texfunc

template<typename InputType, typename OutputType, typename OperatorType>
requires texfunc::CUnaryOperator<OperatorType, InputType, OutputType>
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

}// end namespace ph
