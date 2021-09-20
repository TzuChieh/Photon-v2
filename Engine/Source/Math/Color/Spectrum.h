#pragma once

#include "Common/config.h"
#include "Core/Quantity/spectrum_fwd.h"
#include "Core/Quantity/private_Spectrum/LinearSrgbSpectrum.h"
#include "Core/Quantity/private_Spectrum/TSampledSpectrum.h"

namespace ph
{

#if defined(PH_RENDER_MODE_RGB)
	using Spectrum = LinearSrgbSpectrum;

#elif defined(PH_RENDER_MODE_SPECTRAL)
	using Spectrum = SampledSpectrum;

#elif defined(PH_RENDER_MODE_FULL_SPECTRAL)
	// TODO

#else
	using Spectrum = LinearSrgbSpectrum;

#endif

}// end namespace ph

#include "Core/Quantity/private_Spectrum/TAbstractSpectrum.tpp"
#include "Core/Quantity/private_Spectrum/TSampledSpectrum.tpp"
