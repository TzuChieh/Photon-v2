#pragma once

#include "Core/Texture/TTexture.h"

namespace ph
{

template<typename OutputType>
class TConstantTexture : public TTexture<OutputType>
{
public:
	explicit TConstantTexture(const OutputType& value);

	void sample(const SampleLocation& sampleLocation, OutputType* out_value) const override;

private:
	OutputType m_value;
};

}// end namespace ph

#include "Core/Texture/TConstantTexture.ipp"