#pragma once

#include "Common/config.h"
#include "Core/Quantity/spectral_strength_fwd.h"
#include "Core/Quantity/private_SpectralStrength/LinearSrgbSpectralStrength.h"
#include "Core/Quantity/private_SpectralStrength/TSampledSpectralStrength.h"

namespace ph
{

#if defined(PH_RENDER_MODE_RGB)
	typedef LinearSrgbSpectralStrength SpectralStrength;

#elif defined(PH_RENDER_MODE_SPECTRAL)
	typedef SampledSpectralStrength SpectralStrength;

#elif defined(PH_RENDER_MODE_FULL_SPECTRAL)
	// TODO

#else
	typedef LinearSrgbSpectralStrength SpectralStrength;

#endif

}// end namespace ph

#include "Core/Quantity/private_SpectralStrength/TAbstractSpectralStrength.tpp"
#include "Core/Quantity/private_SpectralStrength/TSampledSpectralStrength.tpp"
