#pragma once

#include "Math/Color/TSampledSpectrum.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Math/Color/spectral_samples.h"

#include <type_traits>

namespace ph::math
{

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
template<typename U>
inline TSampledSpectrum<COLOR_SPACE, T, Props>::TSampledSpectrum(const TSampledSpectrum<COLOR_SPACE, U, Props>& other) :
	TSampledSpectrum(other.getColorValues())
{}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, Props>::numSamples() noexcept
{
	return Props::NUM_SAMPLES;
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, Props>::minWavelengthNM() noexcept
{
	return Props::MIN_WAVELENGTH_NM;
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline consteval std::size_t TSampledSpectrum<COLOR_SPACE, T, Props>::maxWavelengthNM() noexcept
{
	return Props::MAX_WAVELENGTH_NM;
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline consteval auto TSampledSpectrum<COLOR_SPACE, T, Props>::sampleInverval() noexcept
-> T
{
	return static_cast<T>(maxWavelengthNM() - minWavelengthNM()) / static_cast<T>(N);
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline constexpr auto TSampledSpectrum<COLOR_SPACE, T, Props>::wavelengthRangeOf(const std::size_t sampleIndex) noexcept
-> std::pair<T, T>
{
	return wavelength_range_of<T, Props>(sampleIndex);
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
template<typename U>
inline auto TSampledSpectrum<COLOR_SPACE, T, Props>::makePiecewiseAveraged(
	const T* const    wavelengthsNM,
	const T* const    values,
	const std::size_t numPoints)
-> TSampledSpectrum
{
	return TSampledSpectrum().setColorValues(
		calc_piecewise_averaged<T, U, Props>(
			wavelengthsNM, values, numPoints));
}

}// end namespace ph::math
