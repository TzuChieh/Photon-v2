#pragma once

#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"
#include "Common/assertion.h"

#include <utility>
#include <cstddef>

namespace ph::math
{

/*!
@return Wavelength sample interval of @p SampleProps.
*/
template<typename T, CSpectralSampleProps SampleProps>
constexpr T wavelength_interval_of() noexcept;

/*!
@return Wavelength sample range of the sample with @p sampleIndex.
*/
template<typename T, CSpectralSampleProps SampleProps>
constexpr std::pair<T, T> wavelength_range_of(std::size_t sampleIndex) noexcept;

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
T estimate_samples_energy(const TSpectralSampleValues<T, SampleProps>& srcSamples);

/*! @brief Normalize spectral samples as if they carry energy.
Normalized spectral samples, together, represents the expected amount of energy that 1 watt of 
total energy would distribute on each wavelength interval (implying that the samples should sum to 1).
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> normalize_samples_energy(const TSpectralSampleValues<T, SampleProps>& srcSamples);

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
constexpr TSpectralSampleValues<T, SampleProps> constant_spectral_samples(T constant);

template<typename T, typename U, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_spectral_samples(
	const U*          wavelengthsNM,
	const U*          values,
	std::size_t       numPoints,
	ESpectralResample algorithm = ESpectralResample::Default);

/*! @brief SPD of standard illuminants E, with total energy = 1.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_illuminant_E();

/*! @brief SPD of standard illuminants D65, with total energy = 1.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_illuminant_D65();

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_black_body_radiance(T temperatureK);

/*! @brief Converting spectral samples to CIE-XYZ using standard CMFs.
@tparam NORMALIZER Pick a reference white as a normalization target. This will ensure a normalized
SPD of the associated standard illuminant get the corresponding standard white point defined in 
CIE-XYZ. This normalization only take place if the color usage is related to illumination.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps, EReferenceWhite NORMALIZER = EReferenceWhite::D65>
TTristimulusValues<T> spectral_samples_to_CIE_XYZ(const TSpectralSampleValues<T, SampleProps>& srcSamples, EColorUsage usage);

}// end namespace ph::math

#include "Math/Color/spectral_samples.ipp"