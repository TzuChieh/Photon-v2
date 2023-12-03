#pragma once

#include "Math/TVector2.h"
#include "Math/constant.h"
#include "Math/time.h"

#include <cmath>

namespace ph::math
{

/*! @brief Locate spherical coordinates of the sun in sky.

The equation is from Appendix A.6 of the paper "A Practical Analytic Model 
for Daylight" by Preetham et al., 

@param solarTime24H Local solar time in decimal hours.
@param julianDate The day of the year as an integer in the range [1, 366].
@param siteLatitudeRadians Latitude of the site of interest, in radians.
@return Spherical coordinates of the sun in sky.
*/
template<typename T>
inline TVector2<T> sun_sky_phi_theta(
	const T solarTime24H,
	const T julianDate,
	const T siteLatitudeRadians)
{
	using namespace ph::math::constant;

	// Approximated solar declination in radians
	const T delta = 
		static_cast<T>(0.4093) * std::sin(two_pi<T> * (julianDate - static_cast<T>(81)) / static_cast<T>(368));

	const T sinDelta    = std::sin(delta);
	const T cosDelta    = std::cos(delta);
	const T sinLatitude = std::sin(siteLatitudeRadians);
	const T cosLatitude = std::cos(siteLatitudeRadians);

	const T angleTerm    = pi<T> * solarTime24H / static_cast<T>(12);
	const T sinAngleTerm = std::sin(angleTerm);
	const T cosAngleTerm = std::cos(angleTerm);

	const T sunPhiPreetham = std::atan((-cosDelta * sinAngleTerm) / (cosLatitude * sinDelta - sinLatitude * cosDelta * cosAngleTerm));
	const T sunTheta       = pi<T> / static_cast<T>(2) - std::asin(sinLatitude * sinDelta - cosLatitude * cosDelta * cosAngleTerm);

	// Note that <sunPhiPreetham> here has (CW order)
	// 0 = south, pi/2 = west, pi = north and -pi/2 = east; 
	// convert it to engine order here.
	T sunPhi = -sunPhiPreetham + static_cast<T>(3 / 2) * pi<T>;
	sunPhi = sunPhi > two_pi<T> ? sunPhi - two_pi<T> : sunPhi;

	return {sunPhi, sunTheta};
}

}// end namespace ph::math
