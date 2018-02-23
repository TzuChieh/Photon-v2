#pragma once

#include "Core/Quantity/SpectralStrength/LinearSrgbSpectralStrength.h"
#include "Core/Quantity/SpectralStrength/TSampledSpectralStrength.h"
#include "Core/Quantity/spectral_strength_fwd.h"
#include "Common/config.h"

namespace ph
{

#ifdef PH_RENDER_MODE_RGB
	typedef LinearSrgbSpectralStrength SpectralStrength;

#elif PH_RENDER_MODE_SPECTRAL
	typedef SampledSpectralStrength SpectralStrength;

#elif PH_RENDER_MODE_FULL_SPECTRAL
	// TODO

#else
	typedef RgbSpectralStrength SpectralStrength;

#endif

}// end namespace ph