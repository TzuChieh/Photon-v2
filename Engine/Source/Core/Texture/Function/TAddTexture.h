#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"
#include "Core/Texture/SampleLocation.h"
#include "Common/assertion.h"

namespace ph
{

template
<
	typename InputType, 
	typename AdderType, 
	typename OutputType
>
class TAddTexture : public TTextureFunction<InputType, OutputType>
{
public:
	inline TAddTexture() :
		TAddTexture(AdderType(1))
	{}

	inline explicit TAddTexture(const AdderType& adder) :
		m_adder(adder)
	{}

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		static_assert(has_add_operator<InputType, AdderType, OutputType>::value, 
		              "must have add operator for OutputType = InputType + MultiplierType");

		const TTexture<InputType>* inputTexture = this->getInputTexture();
		PH_ASSERT(inputTexture);

		InputType inputValue;
		inputTexture->sample(sampleLocation, &inputValue);

		PH_ASSERT(out_value);
		*out_value = inputValue + m_adder;
	}

	inline TAddTexture& setAdder(const AdderType& adder)
	{
		m_adder = adder;
		return *this;
	}

private:
	AdderType m_adder;
};

}// end namespace ph
