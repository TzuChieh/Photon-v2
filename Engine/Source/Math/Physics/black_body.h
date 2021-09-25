#pragma once

#include "Common/assertion.h"
#include "Math/constant.h"
#include "Math/math.h"

#include <cmath>
#include <cstddef>
#include <vector>
#include <utility>

namespace ph::math
{

/*! @brief Get Black-body radiance at specific temperature and wavelength.
@param temperatureK Temperature in Kelvin.
@param wavelengthNM Wavelength of the interest, in nanometers.
@return Emitted radiance on specified parameters.
*/
template<typename T>
inline T black_body_radiance_at(const T temperatureK, const T wavelengthNM)
{
	PH_ASSERT_GE(temperatureK, 0);
	PH_ASSERT_GT(wavelengthNM, 0);

	if(temperatureK == 0)
	{
		return 0;
	}

	// Using double for calculation as the values are pretty small

	using namespace constant;

	const double nume    = 2.0 * h_Planck<double> * c_light<double> * c_light<double>;

	const double lambda  = wavelengthNM * 1e-9;// convert nm to m
	const double lambda5 = (lambda * lambda) * (lambda * lambda) * lambda;
	const double exp     = (h_Planck<double> * c_light<double>) / (lambda * k_Boltzmann<double> * temperatureK);
	const double deno    = lambda5 * (std::exp(exp) - 1.0);

	return static_cast<T>(nume / deno);
}

/*! @brief Get a curve for Black-body radiation.
@param lambdaMinNM Minimum wavelength of the curve data, in nanometers.
@param lambdaMaxNM Maximum wavelength of the curve data, in nanometers.
@param numCurvePoints Number of points for the curve. Evenly spaced in [lambdaMinNM, lambdaMaxNM].
@return Emitted radiance on each wavelength.
*/
template<typename T>
std::vector<T> black_body_radiance_curve(
	const T               temperatureK,
	const T               lambdaMinNM,
	const T               lambdaMaxNM,
	const std::size_t     numCurvePoints,
	std::vector<T>* const out_lambdaValues = nullptr)
{
	PH_ASSERT_GE(numCurvePoints, 2);
	PH_ASSERT_GT(lambdaMaxNM, lambdaMinNM);

	auto lambdaValues = evenly_spaced_vector<T>(lambdaMinNM, lambdaMaxNM, numCurvePoints);

	std::vector<T> radianceCurve(numCurvePoints, 0);
	for(std::size_t i = 0; i < numCurvePoints; ++i)
	{
		radianceCurve[i] = black_body_radiance_at(temperatureK, lambdaValues[i]);
	}

	if(out_lambdaValues)
	{
		*out_lambdaValues = std::move(lambdaValues);
	}

	return radianceCurve;
}

}// end namespace ph::math
