#pragma once

#include "Common/config.h"
#include "Core/Quantity/spectral_strength_fwd.h"
#include "Core/Quantity/private_SpectralStrength/LinearSrgbSpectralStrength.h"
#include "Core/Quantity/private_SpectralStrength/TSampledSpectralStrength.h"

namespace ph
{

#if defined(PH_RENDER_MODE_RGB)
	using SpectralStrength = LinearSrgbSpectralStrength;

#elif defined(PH_RENDER_MODE_SPECTRAL)
	using SpectralStrength = SampledSpectralStrength;

#elif defined(PH_RENDER_MODE_FULL_SPECTRAL)
	// TODO

#else
	using SpectralStrength = LinearSrgbSpectralStrength;

#endif

}// end namespace ph

#include "Core/Quantity/private_SpectralStrength/TAbstractSpectralStrength.tpp"
#include "Core/Quantity/private_SpectralStrength/TSampledSpectralStrength.tpp"
