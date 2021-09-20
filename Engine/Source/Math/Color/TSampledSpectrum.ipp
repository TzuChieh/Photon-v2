#pragma once

#include "Math/Color/TSampledSpectrum.h"
#include "Common/config.h"
#include "Common/assertion.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"

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

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline consteval auto TSampledSpectrum<COLOR_SPACE, T, N>::sampleInverval() noexcept
-> T
{
	static_assert(maxWavelengthNM() > minWavelengthNM());
	static_assert(N > 0);

	return static_cast<T>(maxWavelengthNM() - minWavelengthNM()) / static_cast<T>(N);
}

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
inline constexpr auto TSampledSpectrum<COLOR_SPACE, T, N>::wavelengthRangeOf(const std::size_t sampleIndex) noexcept
-> std::pair<T, T>
{
	return 
	{
		static_cast<T>(minWavelengthNM()) + static_cast<T>(sampleIndex + 0) * sampleInverval(),
		static_cast<T>(minWavelengthNM()) + static_cast<T>(sampleIndex + 1) * sampleInverval()
	};
}

template<EColorSpace COLOR_SPACE, typename T, std::size_t N>
template<typename U>
inline auto TSampledSpectrum<COLOR_SPACE, T, N>::makePiecewiseAveraged(
	const T* const    wavelengthsNM,
	const T* const    values,
	const std::size_t numPoints)
-> TSampledSpectrum
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
	// (note that <numPoints> does not necessarily equal to <numSamples>)

	TRawColorValues<T, N>            sampled;
	math::TAnalyticalIntegrator1D<T> areaCalculator;
	for(std::size_t i = 0; i < TSampledSpectrum::numSamples(); ++i)
	{
		const auto& range = TSampledSpectrum::wavelengthRangeOf(i);

		areaCalculator.setIntegrationDomain(range.first, range.second);

		const T area         = areaCalculator.integrate(curve);
		const T averageValue = area / (range.second - range.first);
		sampled[i] = averageValue;
	}
	
	return TSampledSpectrum().setColorValues(sampled);
}

}// end namespace ph::math
