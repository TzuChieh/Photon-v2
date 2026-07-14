#pragma once

#include "Engine/Math/Color/color_basics.h"

#include <Common/config.h>

#include <cstddef>
#include <array>
#include <type_traits>

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

/*! @brief Clor space used by @ref Spectrum.
*/
inline constexpr EColorSpace working_color_space = PH_WORKING_COLOR_SPACE_ENUM;

static_assert(
	working_color_space == EColorSpace::Linear_sRGB ||
	working_color_space == EColorSpace::ACEScg ||
	working_color_space == EColorSpace::Spectral,
	"PH_WORKING_COLOR_SPACE must be Linear_sRGB, ACEScg, or Spectral.");

/*! @brief @ref Spectrum type under the working color space.
*/
using Spectrum = std::conditional_t<
	working_color_space == EColorSpace::Spectral,
	SampledSpectrum,
	TTristimulusSpectrum<working_color_space, ColorValue>>;

}// end namespace ph::math
