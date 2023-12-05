#pragma once

#include "Math/Color/TSampledSpectrum.h"
#include "Math/Color/spectral_samples.h"

#include <Common/assertion.h>
#include <Common/config.h>

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
inline constexpr auto TSampledSpectrum<COLOR_SPACE, T, Props>::sampleInterval() noexcept
-> T
{
	return wavelength_interval_of<T, Props>();
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
inline constexpr auto TSampledSpectrum<COLOR_SPACE, T, Props>::wavelengthRangeOf(const std::size_t sampleIndex) noexcept
-> std::pair<T, T>
{
	return wavelength_range_of<T, Props>(sampleIndex);
}

template<EColorSpace COLOR_SPACE, typename T, CSpectralSampleProps Props>
template<typename U>
inline auto TSampledSpectrum<COLOR_SPACE, T, Props>::makeResampled(
	const U* const    wavelengthsNM,
	const U* const    values,
	const std::size_t numPoints)
-> TSampledSpectrum
{
	return TSampledSpectrum().setColorValues(
		resample_spectral_samples<T, U, Props>(
			wavelengthsNM, values, numPoints));
}

}// end namespace ph::math
