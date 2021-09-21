#pragma once

#include "Math/Color/color_basics.h"
#include "Common/assertion.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"

#include <cstddef>
#include <utility>

namespace ph::math
{

template<typename T, CSpectralSampleProps SampleProps>
inline constexpr auto wavelength_range_of(const std::size_t sampleIndex) noexcept
-> std::pair<T, T>
{
	constexpr auto SAMPLE_INTERVAL = 
		static_cast<T>(SampleProps::MAX_WAVELENGTH_NM - SampleProps::MIN_WAVELENGTH_NM) / 
		static_cast<T>(SampleProps::NUM_SAMPLES);

	return
	{
		static_cast<T>(SampleProps::MIN_WAVELENGTH_NM) + static_cast<T>(sampleIndex + 0) * SAMPLE_INTERVAL,
		static_cast<T>(SampleProps::MIN_WAVELENGTH_NM) + static_cast<T>(sampleIndex + 1) * SAMPLE_INTERVAL
	};
}

template<typename T, typename U, CSpectralSampleProps TargetSampleProps>
inline TRawColorValues<T, TargetSampleProps::NUM_SAMPLES> calc_piecewise_averaged(
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
	// (note that <numPoints> does not necessarily equal to <TargetSampleProps::NUM_SAMPLES>)

	using SampledValues = TRawColorValues<T, TargetSampleProps::NUM_SAMPLES>;

	SampledValues sampled;
	math::TAnalyticalIntegrator1D<T> areaCalculator;
	for(std::size_t i = 0; i < TargetSampleProps::NUM_SAMPLES; ++i)
	{
		const auto& range = wavelength_range_of<T, TargetSampleProps>(i);

		areaCalculator.setIntegrationDomain(range.first, range.second);

		const T area         = areaCalculator.integrate(curve);
		const T averageValue = area / (range.second - range.first);
		sampled[i] = averageValue;
	}
	
	return sampled;
}

}// end namespace ph::math
