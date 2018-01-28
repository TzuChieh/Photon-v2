#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"
#include "Core/Texture/SampleLocation.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

template
<
	typename InputType, 
	typename MultiplierType, 
	typename OutputType
	//typename = std::enable_if_t<(has_multiply_operator<InputType, MultiplierType, OutputType>::value &&
	//                             "must have multiply operator for InputType * MultiplierType = OutputType")>
>
class TMultiplyTexture : public TTextureFunction<InputType, OutputType>
{
public:
	inline TMultiplyTexture() :
		TMultiplyTexture(MultiplierType(1))
	{

	}

	inline explicit TMultiplyTexture(const MultiplierType& multiplier) : 
		m_multiplier(multiplier)
	{

	}

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		static_assert(has_multiply_operator<InputType, MultiplierType, OutputType>::value, 
		              "must have multiply operator for InputType * MultiplierType = OutputType");

		const TTexture<InputType>* inputTexture = this->getInputTexture();
		PH_ASSERT(inputTexture != nullptr);

		InputType inputValue;
		inputTexture->sample(sampleLocation, &inputValue);

		PH_ASSERT(out_value != nullptr);
		*out_value = inputValue * m_multiplier;
	}

	inline TMultiplyTexture& setMultiplier(const MultiplierType& multiplier)
	{
		m_multiplier = multiplier;
		return *this;
	}

private:
	MultiplierType m_multiplier;
};

}// end namespace ph