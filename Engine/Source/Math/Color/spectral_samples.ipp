#pragma once

#include "Math/Color/spectral_samples.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"

namespace ph::math
{

template<typename T, CSpectralSampleProps SampleProps>
inline constexpr T wavelength_interval_of() noexcept
{
	return static_cast<T>(SampleProps::MAX_WAVELENGTH_NM - SampleProps::MIN_WAVELENGTH_NM)) / 
	       static_cast<T>(SampleProps::NUM_SAMPLES);
}

template<typename T, CSpectralSampleProps SampleProps>
inline constexpr auto wavelength_range_of(const std::size_t sampleIndex) noexcept
-> std::pair<T, T>
{
	constexpr auto INTERVAL_NM = wavelength_interval_of<T, SampleProps>();

	return
	{
		static_cast<T>(SampleProps::MIN_WAVELENGTH_NM) + static_cast<T>(sampleIndex + 0) * INTERVAL_NM,
		static_cast<T>(SampleProps::MIN_WAVELENGTH_NM) + static_cast<T>(sampleIndex + 1) * INTERVAL_NM
	};
}

template<typename T, typename U, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> make_piecewise_avg_spectral_samples(
	const U* const    wavelengthsNM,
	const U* const    values,
	const std::size_t numPoints)
{
	PH_ASSERT(wavelengthsNM);
	PH_ASSERT(values);

	// Construct a curve from specified points

	math::TPiecewiseLinear1D<T> curve;
	for(std::size_t i = 0; i < numPoints; i++)
	{
		const T wavelengthNm = wavelengthsNM[i];
		const T value        = values[i];

		curve.addPoint({wavelengthNm, value});
	}
	curve.update();

	// Sample curve values by averaging each wavelength interval
	// (note that <numPoints> does not necessarily equal to <SampleProps::NUM_SAMPLES>)

	TSpectralSampleValues<T, SampleProps> sampled;
	math::TAnalyticalIntegrator1D<T>      areaCalculator;
	for(std::size_t i = 0; i < SampleProps::NUM_SAMPLES; ++i)
	{
		const auto& range = wavelength_range_of<T, SampleProps>(i);

		areaCalculator.setIntegrationDomain(range.first, range.second);

		const T area         = areaCalculator.integrate(curve);
		const T averageValue = area / (range.second - range.first);
		sampled[i] = averageValue;
	}
	
	return sampled;
}

//template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
//inline TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_E()
//{
//	static 
//}
//
//template<typename T, CSpectralSampleProps SampleProps = DefaultSpectralSampleProps>
//inline TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_D65();

}// end namespace ph::math
