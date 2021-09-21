#pragma once

#include "Math/Color/color_basics.h"
#include "Common/assertion.h"

#include <utility>
#include <cstddef>

namespace ph::math
{

template<typename T, CSpectralSampleProps SampleProps>
constexpr T wavelength_interval_of() noexcept;

template<typename T, CSpectralSampleProps SampleProps>
constexpr std::pair<T, T> wavelength_range_of(std::size_t sampleIndex) noexcept;

template<typename T, typename U, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_piecewise_avg_spectral_samples(
	const U*    wavelengthsNM,
	const U*    values,
	std::size_t numPoints);

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_E();

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_D65();

}// end namespace ph::math

#include "Math/Color/spectral_samples.ipp"
