#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"

#include <cstddef>

namespace ph
{

class SpectralData
{
public:
	static SampledSpectralStrength calcPiecewiseAveraged(
		const real* wavelengthsNm, const real* values, std::size_t numPoints);
};

}// end namespace ph