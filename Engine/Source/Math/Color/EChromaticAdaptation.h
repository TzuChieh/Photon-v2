#pragma once

namespace ph::math
{

/*! @brief Methods to map image appearance between different illumination sources.
This kind of mapping is commonly called Chromatic Adaptation Transform (CAT).
*/
enum class EChromaticAdaptation
{
	UNSPECIFIED = 0,

	/*! The most simple transform. Generally considered to be an inferior CAT. */
	XYZScaling,

	/*! Adobe uses this CAT in all of their products according to many sources. */
	Bradford,

	VonKries,

	// TODO: https://en.wikipedia.org/wiki/CIECAM02
	//CAT02

	// TODO: spectral route
};

}// end namespace ph::math
