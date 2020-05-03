#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/Spectrum.h"

#include <cstddef>

namespace ph
{

class SpectralData
{
public:
	static SampledSpectrum calcPiecewiseAveraged(
		const real* wavelengthsNm, const real* values, std::size_t numPoints);
};

}// end namespace ph
