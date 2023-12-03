#pragma once

#include "Common/config.h"
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
	EColorSpace::Spectral,
	ColorValue, 
	DefaultSpectralSampleProps>;

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

}// end namespace ph::math
