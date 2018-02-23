#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Core/Quantity/ColorSpace/spectral_data.h"
#include "Core/Quantity/SpectralStrength.h"

#include <array>
#include <iostream>

namespace ph
{

namespace
{

static SampledSpectralStrength kernel_X_D65;
static SampledSpectralStrength kernel_Y_D65;
static SampledSpectralStrength kernel_Z_D65;

static SampledSpectralStrength SPD_D65;

static SampledSpectralStrength make_sampled(
	const real* const wavelengthsNm, 
	const real* const values, 
	const std::size_t numPoints)
{
	PH_ASSERT(wavelengthsNm != nullptr && values != nullptr);

	// construct a curve from specified points
	//
	TPiecewiseLinear1D<real> curve;
	for(std::size_t i = 0; i < numPoints; i++)
	{
		const real wavelengthNm = wavelengthsNm[i];
		const real value        = values[i];

		curve.addPoint({wavelengthNm, value});
	}
	curve.update();

	// sample curve values by averaging each wavelength interval
	//
	SampledSpectralStrength       sampled;
	TAnalyticalIntegrator1D<real> areaCalculator;
	for(std::size_t i = 0; i < SampledSpectralStrength::NUM_INTERVALS; i++)
	{
		const auto& range = SampledSpectralStrength::lambdaRangeNmOf(i);

		areaCalculator.setIntegrationDomain(range.first, range.second);

		const real area         = areaCalculator.integrate(curve);
		const real averageValue = area / (range.second - range.first);
		sampled[i] = averageValue;
	}
	
	return sampled;
}

}// end anonymous namespace

void ColorSpace::init()
{
	PH_ASSERT_MSG(!isInitialized(), "ColorSpace is already initialized");

	// Construct sampled D65 spectrum and normalize it by making its largest
	// value equals to 1.
	//
	SPD_D65 = make_sampled(spectral_data::CIE_D65_wavelengths_nm().data(),
	                       spectral_data::CIE_D65_values().data(),
	                       std::tuple_size<spectral_data::ArrayD65>::value);
	SPD_D65.divLocal(SPD_D65.max());

	// Sample XYZ color matching functions first, then later normalize it so 
	// that dotting them with sampled D65 spectrum is equivalent to computing
	// (X, Y, Z) tristimulus values and will yield (0.95047, 1, 1.08883).

	// sampling
	//
	const std::size_t numXyzCmfPoints = std::tuple_size<spectral_data::ArrayXYZCMF>::value;
	kernel_X_D65 = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                            spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
	                            numXyzCmfPoints);
	kernel_Y_D65 = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                            spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
	                            numXyzCmfPoints);
	kernel_Z_D65 = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                            spectral_data::XYZ_CMF_CIE_1931_2_degree_Z().data(), 
	                            numXyzCmfPoints);

	// normalizing

	// multiplier of Riemann Sum
	//
	kernel_X_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Y_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	kernel_Z_D65.mulLocal(SampledSpectralStrength::LAMBDA_INTERVAL_NM);
	
	// scaled to match D65
	//
	kernel_X_D65.mulLocal(0.95047_r / kernel_X_D65.dot(SPD_D65));
	kernel_Y_D65.mulLocal(1.00000_r / kernel_Y_D65.dot(SPD_D65));
	kernel_Z_D65.mulLocal(1.08883_r / kernel_Z_D65.dot(SPD_D65));

	PH_ASSERT(isInitialized(true));
}

Vector3R ColorSpace::SPD_to_CIE_XYZ_D65(const SampledSpectralStrength& spd)
{
	PH_ASSERT(isInitialized());

	return Vector3R(kernel_X_D65.dot(spd),
	                kernel_Y_D65.dot(spd),
	                kernel_Z_D65.dot(spd));
}

const SampledSpectralStrength& ColorSpace::get_D65_SPD()
{
	PH_ASSERT(isInitialized());

	return SPD_D65;
}

}// end namespace ph