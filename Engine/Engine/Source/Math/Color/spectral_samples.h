#pragma once

#include "Math/Color/color_enums.h"
#include "Math/Color/color_basics.h"

#include <Common/assertion.h>

#include <utility>
#include <cstddef>

namespace ph::math
{

/*!
@return Wavelength sample interval of @p SampleProps, in nanometers (nm).
*/
template<typename T, CSpectralSampleProps SampleProps>
constexpr T wavelength_interval_of() noexcept;

/*!
@return Wavelength sample range of the sample with @p sampleIndex, in nanometers (nm).
*/
template<typename T, CSpectralSampleProps SampleProps>
constexpr std::pair<T, T> wavelength_range_of(std::size_t sampleIndex) noexcept;

template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
T estimate_samples_energy(const TSpectralSampleValues<T, SampleProps>& srcSamples);

/*! @brief Normalize spectral samples as if they carry energy.
Normalized spectral samples, together, represents the expected amount of energy that 1 unit of 
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

/*! @brief SPD of standard illuminants.
Any light source which statistically has the same relative SPD as a standard illuminant can be 
considered a standard light source, and is the reason why the SPD resample functions return a 
energy-normalized SPD (for convenience, as this is what you want almost always).
*/
///@{
/*! @brief SPD of standard illuminants E, with total energy = 1.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_illuminant_E();

/*! @brief SPD of standard illuminants D65, with total energy = 1.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_illuminant_D65();
///@}

/*! @brief SPD of black-body radiation, with total energy = 1.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_black_body(T temperatureK);

/*! @brief SPD of black-body radiation in radiance.
If a normalized energy distribution is desired, use `resample_black_body()`.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_black_body_radiance(T temperatureK);

/*! @brief SPD of black-body radiation in spectral radiance.
Note that this function is not returning radiance but spectral radiance. The returned
spectral radiance has the unit of watt per steradian per square meter per meter (W * sr^-1 * m^-2 * m^-1).
If radiance is desired, use `resample_black_body_radiance()`. If a normalized energy distribution is
desired, use `resample_black_body()`.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
TSpectralSampleValues<T, SampleProps> resample_black_body_spectral_radiance(T temperatureK);

/*! @brief Converting spectral samples to CIE-XYZ using standard CMFs.
@tparam NORMALIZER Pick a reference white as a normalization target. This will ensure a normalized
SPD of the associated standard illuminant get the corresponding standard white point defined in 
CIE-XYZ. This normalization only take place if the color usage is related to illumination.
*/
template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps, EReferenceWhite NORMALIZER = EReferenceWhite::D65>
TTristimulusValues<T> spectral_samples_to_CIE_XYZ(const TSpectralSampleValues<T, SampleProps>& srcSamples, EColorUsage usage);

}// end namespace ph::math

#include "Math/Color/spectral_samples.ipp"
