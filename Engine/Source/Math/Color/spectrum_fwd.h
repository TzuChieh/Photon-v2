#pragma once

#include "Math/Color/color_basics.h"

#include <cstddef>
#include <array>

namespace ph::math
{

template<typename Derived, EColorSpace COLOR_SPACE, typename T, std::size_t N>
class TSpectrumBase;

template<EColorSpace COLOR_SPACE, typename T>
class TTristimulusSpectrum;

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
class TSampledSpectrum;

using LinearSRGBSpectrum = TTristimulusSpectrum<EColorSpace::Linear_sRGB, ColorValue>;

using ACESSpectrum = TTristimulusSpectrum<EColorSpace::ACEScg, ColorValue>;

using SampledSpectrum = TSampledSpectrum<
	EColorSpace::Spectral_Smits, 
	ColorValue, 
	DefaultSpectralSampleProps>;

}// end namespace ph::math
