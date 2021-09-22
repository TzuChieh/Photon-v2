#pragma once

#include "Math/Color/spectral_samples.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Math/TArithmeticArray.h"
#include "Math/Color/spectral_data.h"

#include <array>

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

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> normalize_spectral_energy_samples(const TSpectralSampleValues<T, SampleProps>& srcSamples)
{
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> samples(srcSamples);

	const T sum = samples.sum();
	if(sum > 0)
	{
		samples.divLocal(sum);
	}

	return samples.toArray();
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

	math::TPiecewiseLinear1D<U> curve;
	for(std::size_t i = 0; i < numPoints; i++)
	{
		const U wavelengthNm = wavelengthsNM[i];
		const U value        = values[i];

		curve.addPoint({wavelengthNm, value});
	}
	curve.update();

	// Sample curve values by averaging each wavelength interval
	// (note that <numPoints> does not necessarily equal to <SampleProps::NUM_SAMPLES>)

	TSpectralSampleValues<T, SampleProps> sampled;
	math::TAnalyticalIntegrator1D<U>      areaCalculator;
	for(std::size_t i = 0; i < SampleProps::NUM_SAMPLES; ++i)
	{
		const auto& range = wavelength_range_of<U, SampleProps>(i);

		areaCalculator.setIntegrationDomain(range.first, range.second);

		const U area     = areaCalculator.integrate(curve);
		const U avgValue = area / (range.second - range.first);
		sampled[i] = static_cast<T>(avgValue);
	}
	
	return sampled;
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_E()
{
	TSpectralSampleValues<T, SampleProps> samples;
	samples.fill(1);
	return normalize_spectral_energy_samples(samples);
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> make_sampled_illuminant_D65()
{
	auto samples = make_piecewise_avg_spectral_samples<T, spectral_data::ArrayD65::value_type, SampleProps>(
		spectral_data::CIE_D65_wavelengths_nm().data(),
		spectral_data::CIE_D65_values().data(), 
		std::tuple_size_v<spectral_data::ArrayD65>);

	return normalize_spectral_energy_samples(samples);
}

namespace detail
{

template<typename T, CSpectralSampleProps SampleProps>
struct TCIEXYZCmfKernel final
{
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> weights[3];

	inline TCIEXYZCmfKernel()
	{
		// Sample XYZ color matching functions first, then later normalize it so 
		// that dotting them with sampled E spectrum is equivalent to computing
		// (X, Y, Z) tristimulus values and will yield (1, 1, 1).

		// Sampling XYZ CMF

		using XYZCMFValueType = spectral_data::ArrayD65::value_type;
		constexpr auto NMU_XYZ_CMF_POINTS = std::tuple_size_v<spectral_data::ArrayXYZCMF>;

		const auto sampledCmfValuesX = make_piecewise_avg_spectral_samples<T, XYZCMFValueType, SampleProps>(
			spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
			spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
			NMU_XYZ_CMF_POINTS);

		const auto sampledCmfValuesY = make_piecewise_avg_spectral_samples<T, XYZCMFValueType, SampleProps>(
			spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
			spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
			NMU_XYZ_CMF_POINTS);

		const auto sampledCmfValuesZ = make_piecewise_avg_spectral_samples<T, XYZCMFValueType, SampleProps>(
			spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
			spectral_data::XYZ_CMF_CIE_1931_2_degree_Z().data(), 
			NMU_XYZ_CMF_POINTS);

		weights[0].set(sampledCmfValuesX);
		weights[1].set(sampledCmfValuesY);
		weights[2].set(sampledCmfValuesZ);

		// Normalizing

		constexpr T wavelengthIntervalNM = wavelength_interval_of<T, SampleProps>();

		// Integration of CMF-Y by Riemann Sum
		const T integratedCmfY = (weights[1] * wavelengthIntervalNM).sum();

		for(std::size_t ci = 0; ci < 3; ++ci)
		{
			// Multiplier of Riemann Sum and denominator
			// (after this, the weights are usable, but may need further refinements)
			weights[ci] = (weights[ci] * wavelengthIntervalNM) / integratedCmfY;

			// Energy normalizing with respect to illuminant E (equal energy spectrum)
			// (one can also normalize using other illuminants such as D65; we chose E here
			// since we would like to keep energy conservative fractions well within [0, 1])

			const T illuminantESamples = make_sampled_illuminant_E<T, SampleProps>();

			T integrationResult = 0;
			for(std::size_t si = 0; si < SampleProps::NUM_SAMPLES; ++si)
			{
				integrationResult += weights[ci][si] * illuminantESamples[si];
			}

			// Normalize the weights
			PH_ASSERT_GT(integrationResult, 0);
			weights[ci] /= integrationResult;
		}
	}
};

}// end detail

template<typename T, CSpectralSampleProps SampleProps>
inline TTristimulusValues<T> spectral_samples_to_CIE_XYZ(const TSpectralSampleValues<T, SampleProps>& srcSamples)
{
	static const detail::TCIEXYZCmfKernel<T, SampleProps> kernel;

	TTristimulusValues<T> CIEXYZColor;
	for(std::size_t ci = 0; ci < 3; ++ci)
	{
		CIEXYZColor[ci] = 0;
		for(std::size_t si = 0; si < SampleProps::NUM_SAMPLES; ++si)
		{
			CIEXYZColor[ci] += kernel.weights[ci][si] * srcSamples[si];
		}
	}
	return CIEXYZColor;
}

}// end namespace ph::math
