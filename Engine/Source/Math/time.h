#pragma once

#include "Math/constant.h"

#include <cmath>

namespace ph::math
{

/*! @brief Convert mean solar time to solar time.

The equation is from Appendix A.6 of the paper "A Practical Analytic Model 
for Daylight" by Preetham et al., 

@param standardTime24H Local standard time in decimal hours.
@param standardMeridianRadians Standard meridian used for local standard time, in radians.
@param siteLongitudeRadians Longitude of the site of interest, in radians.
@param julianDate The day of the year as an integer in the range [1, 366].
@return Solar time in decimal hours.
*/
template<typename T>
inline T mean_solar_time_to_solar_time_24H(
	const T standardTime24H,
	const T standardMeridianRadians,
	const T siteLongitudeRadians,
	const T julianDate)
{
	using namespace ph::math::constant;

	const T term1 = static_cast<T>(0.170) * std::sin(four_pi<T> * (julianDate - static_cast<T>(80)) / static_cast<T>(373));
	const T term2 = static_cast<T>(-0.129) * std::sin(two_pi<T> * (julianDate - static_cast<T>(8)) / static_cast<T>(355));
	const T term3 = static_cast<T>(12) * (standardMeridianRadians - siteLongitudeRadians) * rcp_pi<T>;

	return standardTime24H + term1 + term2 + term3;
}

}// end namespace ph::math
