#pragma once

#include "Common/config.h"
#include "Math/Color/spectrum_fwd.h"
#include "Math/Color/TTristimulusSpectrum.h"
#include "Math/Color/TSampledSpectrum.h"

namespace ph::math
{

#if defined(PH_RENDER_MODE_LINEAR_SRGB)

	using Spectrum = LinearSRGBSpectrum;

#elif defined(PH_RENDER_MODE_ACES)

	using Spectrum = ACESSpectrum;

#elif defined(PH_RENDER_MODE_SPECTRAL)

	using Spectrum = SampledSpectrum;

#elif defined(PH_RENDER_MODE_FULL_SPECTRAL)

	// TODO

#else

	using Spectrum = LinearSRGBSpectrum;

#endif

static_assert(CColorValuesInterface<Spectrum>);
static_assert(CColorValuesInterface<LinearSRGBSpectrum>);
static_assert(CColorValuesInterface<ACESSpectrum>);
static_assert(CColorValuesInterface<SampledSpectrum>);

}// end namespace ph::math
