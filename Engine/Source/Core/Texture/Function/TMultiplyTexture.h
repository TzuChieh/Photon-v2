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
	typename OutputType,
	typename = typename std::enable_if<has_multiply_operator<InputType, MultiplierType, OutputType>::value>::type
>
class TMultiplyTexture : public TTextureFunction<InputType, OutputType>
{
public:
	inline TMultiplyTexture() :
		TMultiplyTexture(MultiplierType(1))
	{

	}

	inline TMultiplyTexture(const MultiplierType& multiplier) : 
		m_multiplier(multiplier)
	{

	}

	inline virtual void sample(const SampleLocation& sampleLocation, OutputType* const out_value) const override
	{
		PH_ASSERT(out_value != nullptr);
		
		const TTexture<InputType>* parentTexture = this->getParentTexture();
		PH_ASSERT(parentTexture != nullptr);

		InputType inputValue;
		parentTexture->sample(sampleLocation, &inputValue);
		*out_value = inputValue * m_multiplier;
	}

private:
	MultiplierType m_multiplier;
};

}// end namespace ph