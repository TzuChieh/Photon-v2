#include "Core/Quantity/ColorSpace.h"
#include "Common/assertion.h"
#include "Math/Function/TPiecewiseLinear1D.h"
#include "Math/Solver/TAnalyticalIntegrator1D.h"
#include "Core/Quantity/ColorSpace/spectral_data.h"

#include <array>
#include <iostream>

namespace ph
{

SampledSpectralStrength ColorSpace::kernel_X;
SampledSpectralStrength ColorSpace::kernel_Y;
SampledSpectralStrength ColorSpace::kernel_Z;

namespace
{

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

	const std::size_t numPoints = std::tuple_size<spectral_data::ArrayXYZCMF>::value;

	kernel_X = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                        spectral_data::XYZ_CMF_CIE_1931_2_degree_X().data(), 
	                        numPoints);
	kernel_Y = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                        spectral_data::XYZ_CMF_CIE_1931_2_degree_Y().data(), 
	                        numPoints);
	kernel_Z = make_sampled(spectral_data::XYZ_CMF_CIE_1931_2_degree_wavelengths_nm().data(),
	                        spectral_data::XYZ_CMF_CIE_1931_2_degree_Z().data(), 
	                        numPoints);

	// TODO

	PH_ASSERT(isInitialized(true));
}

}// end namespace ph