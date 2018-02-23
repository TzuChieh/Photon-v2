#pragma once

#include "Core/Quantity/SpectralStrength/RgbSpectralStrength.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"
#include "Common/config.h"

namespace ph
{

typedef TSampledSpectralStrength
<
	PH_SPECTRUM_SAMPLED_NUM_SAMPLES,
	PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM, 
	PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM
> SampledSpectralStrength;

#ifdef PH_RENDER_MODE_RGB
	typedef RgbSpectralStrength SpectralStrength;

#elif PH_RENDER_MODE_SPECTRAL
	typedef SampledSpectralStrength SpectralStrength;

#elif PH_RENDER_MODE_FULL_SPECTRAL
	// TODO

#else
	typedef RgbSpectralStrength SpectralStrength;

#endif

}// end namespace ph