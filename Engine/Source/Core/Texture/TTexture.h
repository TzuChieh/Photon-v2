#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/Spectrum.h"

namespace ph
{

class SampleLocation;

template<typename OutputType>
class TTexture
{
public:
	using Output = OutputType;

public:
	inline TTexture() = default;
	inline virtual ~TTexture() = default;

	virtual void sample(const SampleLocation& sampleLocation, Output* out_value) const = 0;
};

}// end namespace ph
