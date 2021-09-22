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

/*! @brief Normalize spectral samples as if they carry energy.
Normalized spectral samples, together, represents the expected amount of energy that 1 watt of 
total energy would distribute on each wavelength interval (implying that the samples should sum to 1).
*/
template<typename T, CSpectralSampleProps SampleProps>
TSpectralSampleValues<T, SampleProps> normalize_spectral_energy_samples(const TSpectralSampleValues<T, SampleProps>& srcSamples);

template<typename T, typename U, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_piecewise_avg_spectral_samples(
	const U*    wavelengthsNM,
	const U*    values,
	std::size_t numPoints);

/*! Normalized SPD of standard illuminants E.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_E();

/*! Normalized SPD of standard illuminants D65.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_D65();

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TTristimulusValues<T> spectral_samples_to_CIE_XYZ(const TSpectralSampleValues<T, SampleProps>& srcSamples);

}// end namespace ph::math

#include "Math/Color/spectral_samples.ipp"
