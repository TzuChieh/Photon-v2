#pragma once

#include "Engine/Math/Color/spectrum_fwd.h"
#include "Engine/Math/Color/TTristimulusSpectrum.h"
#include "Engine/Math/Color/TSampledSpectrum.h"

namespace ph::math
{

// Require complete types for these checks
static_assert(CColorValuesInterface<Spectrum>);
static_assert(CColorValuesInterface<LinearSRGBSpectrum>);
static_assert(CColorValuesInterface<ACESSpectrum>);
static_assert(CColorValuesInterface<SampledSpectrum>);

}// end namespace ph::math
