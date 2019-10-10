#pragma once

#include "Core/Texture/Function/TTextureFunction.h"
#include "Utility/has_member.h"
#include "Common/assertion.h"

#include <memory>

namespace ph
{

template<typename InputType, typename OutputType>
class TConversionTexture : public TTextureFunction<InputType, OutputType>
{
	static_assert(is_buildable<OutputType, InputType>(),
		"OutputType must be buildable from InputType");

public:
	inline TConversionTexture() :
		TTextureFunction<InputType, OutputType>()
	{}

	inline TConversionTexture(const std::shared_ptr<TTexture<InputType>>& input) :
		TTextureFunction<InputType, OutputType>()
	{
		this->setInputTexture(input);
	}

	virtual inline ~TConversionTexture() override = default;

	virtual inline void sample(
		const SampleLocation& sampleLocation, 
		OutputType* const     out_value) const override
	{
		const TTexture<InputType>* inputTexture = this->getInputTexture();
		PH_ASSERT(inputTexture);

		InputType inputValue;
		inputTexture->sample(sampleLocation, &inputValue);

		PH_ASSERT(out_value);
		*out_value = OutputType(inputValue);
	}
};

}// end namespace ph
