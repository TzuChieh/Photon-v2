#pragma once

#include "Math/Color/TSampledSpectrum.h"
#include "Common/config.h"

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<typename U>
inline TSampledSpectrum<COLOR_SPACE, T, N>::TSampledSpectrum(const TSampledSpectrum<COLOR_SPACE, U, N>& other) :
	TSampledSpectrum(other.getColorValues())
{}

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, N>::numSamples() noexcept
{
	return N;
}

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, N>::minWavelengthNM() noexcept
{
	return PH_SPECTRUM_SAMPLED_MIN_WAVELENGTH_NM;
}

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, N>::maxWavelengthNM() noexcept
{
	return PH_SPECTRUM_SAMPLED_MAX_WAVELENGTH_NM;
}

}// end namespace ph::math
