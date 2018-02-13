#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class SampleLocation;

template<typename OutputType>
class TTexture
{
public:
	typedef OutputType Output;

public:
	inline TTexture();
	inline virtual ~TTexture() = 0;

	inline virtual void sample(const SampleLocation& sampleLocation, Output* out_value) const = 0;
};

}// end namespace ph

#include "Core/Texture/TTexture.ipp"