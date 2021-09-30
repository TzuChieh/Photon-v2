#pragma once

#include "Math/Color/spectral_samples.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Math/TArithmeticArray.h"
#include "Math/Color/spectral_data.h"
#include "Math/Physics/black_body.h"
#include "Math/math_exceptions.h"

#include <array>
#include <vector>

namespace ph::math
{

template<typename T, CSpectralSampleProps SampleProps>
inline constexpr T wavelength_interval_of() noexcept
{
	return static_cast<T>(SampleProps::MAX_WAVELENGTH_NM - SampleProps::MIN_WAVELENGTH_NM) / 
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
inline T estimate_samples_energy(const TSpectralSampleValues<T, SampleProps>& srcSamples)
{
	const T sum = TArithmeticArray<T, SampleProps::NUM_SAMPLES>(srcSamples).sum();
	return sum > 0 ? sum : 0;
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> normalize_samples_energy(const TSpectralSampleValues<T, SampleProps>& srcSamples)
{
	TArithmeticArray<T, SampleProps::NUM_SAMPLES> samples(srcSamples);

	const T energy = estimate_samples_energy(srcSamples);
	if(energy > 0)
	{
		samples.divLocal(energy);
	}

	return samples.toArray();
}

template<typename T, typename U, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> resample_spectral_samples(
	const U* const          wavelengthsNM,
	const U* const          values,
	const std::size_t       numPoints,
	const ESpectralResample algorithm)
{
	PH_ASSERT(wavelengthsNM);
	PH_ASSERT(values);
	PH_ASSERT(algorithm != ESpectralResample::UNSPECIFIED);

	TSpectralSampleValues<T, SampleProps> sampled(0);

	if(algorithm == ESpectralResample::PiecewiseAveraged)
	{
		// Construct a curve from specified points
		// TODO: add option for clamp to edge or set as zero, etc. for out of bound samples

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

		math::TAnalyticalIntegrator1D<U> areaCalculator;
		for(std::size_t i = 0; i < SampleProps::NUM_SAMPLES; ++i)
		{
			const auto& range = wavelength_range_of<U, SampleProps>(i);

			areaCalculator.setIntegrationDomain(range.first, range.second);

			const U area     = areaCalculator.integrate(curve);
			const U avgValue = area / (range.second - range.first);
			sampled[i] = static_cast<T>(avgValue);
		}
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	return sampled;
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> resample_illuminant_E()
{
	TSpectralSampleValues<T, SampleProps> samples;
	samples.fill(1);

	return normalize_samples_energy(samples);
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> resample_illuminant_D65()
{
	const auto samples = resample_spectral_samples<T, spectral_data::ArrayD65::value_type, SampleProps>(
		spectral_data::CIE_D65_wavelengths_nm().data(),
		spectral_data::CIE_D65_values().data(), 
		std::tuple_size_v<spectral_data::ArrayD65>);

	return normalize_samples_energy(samples);
}

template<typename T, CSpectralSampleProps SampleProps>
inline TSpectralSampleValues<T, SampleProps> resample_black_body_radiance(const T temperatureK)
{
	std::vector<T> radianceLambdas;
	const std::vector<T> radianceValues = black_body_radiance_curve<double>(
		temperatureK, 
		SampleProps::MIN_WAVELENGTH_NM, 
		SampleProps::MAX_WAVELENGTH_NM, 
		SampleProps::NUM_SAMPLES,
		&radianceLambdas);

	const auto samples = resample_spectral_samples<T, double, SampleProps>(
		radianceLambdas.data(),
		radianceValues.data(),
		radianceValues.size());

	return normalize_samples_energy(samples);
}

namespace detail
{

template<typename T, CSpectralSampleProps SampleProps>
struct TCIEXYZCmfKernel final
{
	using ArrayType = TArithmeticArray<T, SampleProps::NUM_SAMPLES>;

	std::array<ArrayType, 3> weights;
	std::array<T, 3>         illuminantD65Normalizer;

	inline TCIEXYZCmfKernel()
	{
		// Sample XYZ color matching functions first, then later normalize it so 
		// that dotting them with sampled E spectrum is equivalent to computing
		// (X, Y, Z) tristimulus values and will yield (1, 1, 1).

		// Sampling XYZ CMF

		using XYZCMFValueType = spectral_data::ArrayD65::value_type;
		constexpr auto NMU_XYZ_CMF_POINTS = std::tuple_size_v<spectral_data::ArrayXYZCMF>;

		const auto sampledCmfValuesX = resample_spectral_samples<T, XYZCMFValueType, SampleProps>(
			spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
			spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
			NMU_XYZ_CMF_POINTS);

		const auto sampledCmfValuesY = resample_spectral_samples<T, XYZCMFValueType, SampleProps>(
			spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
			spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
			NMU_XYZ_CMF_POINTS);

		const auto sampledCmfValuesZ = resample_spectral_samples<T, XYZCMFValueType, SampleProps>(
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

		const auto uniformUnitSamples   = ArrayType(1);
		const auto illuminantD65Samples = ArrayType(resample_illuminant_D65<T, SampleProps>());
		const auto CIEXYZD65WhitePoint  = CIEXYZ_of<T>(EReferenceWhite::D65);

		for(std::size_t ci = 0; ci < 3; ++ci)
		{
			// Multiplier of Riemann Sum and denominator
			weights[ci] = (weights[ci] * wavelengthIntervalNM) / integratedCmfY;

			// Normalize weights[ci] such that <uniformUnitSamples> will be weighted to 1
			// (sum of weights[ci] should be ~= 1 already, this is equivalent to explicitly make them sum to 1)
			weights[ci] /= weights[ci].dot(uniformUnitSamples);

			// Now, weights[ci] is usable, but may need further refinements depending on usage

			// Normalization multiplier based on a D65 illuminant
			// (this multiplier will ensure a normalized D65 SPD get the corresponding standard white point)
			illuminantD65Normalizer[ci] = CIEXYZD65WhitePoint[ci] / weights[ci].dot(illuminantD65Samples);
		}
	}
};

}// end detail

template<typename T, CSpectralSampleProps SampleProps>
inline TTristimulusValues<T> spectral_samples_to_CIE_XYZ(const TSpectralSampleValues<T, SampleProps>& srcSamples, const EColorUsage usage)
{
	static const detail::TCIEXYZCmfKernel<T, SampleProps> kernel;

	const TArithmeticArray<T, SampleProps::NUM_SAMPLES> copiedSrcSamples(srcSamples);

	TArithmeticArray<T, 3> CIEXYZColor;
	for(std::size_t ci = 0; ci < 3; ++ci)
	{
		CIEXYZColor[ci] = copiedSrcSamples.dot(kernel.weights[ci]);
	}

	switch(usage)
	{
	case EColorUsage::EMR:
		// Note that this multiplier will ensure a normalized D65 SPD get the corresponding standard 
		// white point defined in CIE-XYZ. The multiplier does not meant only for D65-based illuminants.
		// Just that most illuminants are defined with respect to D65, so it is reasonable to "calibrate"
		// the kernel using D65 in this case.
		CIEXYZColor[0] *= kernel.illuminantD65Normalizer[0];
		CIEXYZColor[1] *= kernel.illuminantD65Normalizer[1];
		CIEXYZColor[2] *= kernel.illuminantD65Normalizer[2];
		break;

	case EColorUsage::ECF:
		// The largest possible <srcSamples> in this case is a constant spectrum of value 1--the resulting
		// CIE-XYZ color should always be in [0, 1].
		CIEXYZColor.clampLocal(0, 1);
		break;

	case EColorUsage::RAW:
		// Do nothing
		break;

	default:
		throw ColorError(
			"A color usage must be specified when converting spectral color samples.");
		break;
	}

	return CIEXYZColor.toArray();
}

}// end namespace ph::math
