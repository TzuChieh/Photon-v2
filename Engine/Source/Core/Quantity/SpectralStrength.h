#pragma once

#include "Core/Quantity/SpectralStrength/RgbSpectralStrength.h"
#include "Common/config.h"

namespace ph
{

#ifdef PH_RENDER_MODE_RGB
	typedef RgbSpectralStrength SpectralStrength;

#elif PH_RENDER_MODE_SPECTRAL
	// TODO

#elif PH_RENDER_MODE_FULL_SPECTRAL
	// TODO

#else
	typedef RgbSpectralStrength SpectralStrength;

#endif

}// end namespace ph