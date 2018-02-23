#pragma once

#include "Math/math_fwd.h"
#include "Common/config.h"

namespace ph
{

template<typename DerivedType, std::size_t N>
class TAbstractSpectralStrength;

class LinearSrgbSpectralStrength;

template<std::size_t N, std::size_t MIN_LAMBDA_NM, std::size_t MAX_LAMBDA_NM>
class TSampledSpectralStrength;

typedef TSampledSpectralStrength
<
	PH_SPECTRUM_SAMPLED_NUM_SAMPLES,
	PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM,
	PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM
> SampledSpectralStrength;

}// end namespace ph