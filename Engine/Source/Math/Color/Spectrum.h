#pragma once

#include "Math/Color/spectrum_fwd.h"
#include "Math/Color/TTristimulusSpectrum.h"
#include "Math/Color/TSampledSpectrum.h"

namespace ph::math
{

// Require complete types for these checks
static_assert(CColorValuesInterface<Spectrum>);
static_assert(CColorValuesInterface<LinearSRGBSpectrum>);
static_assert(CColorValuesInterface<ACESSpectrum>);
static_assert(CColorValuesInterface<SampledSpectrum>);

}// end namespace ph::math
