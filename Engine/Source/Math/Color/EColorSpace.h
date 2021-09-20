#pragma once

namespace ph::math
{

enum class EColorSpace
{
	UNSPECIFIED = 0,

	/*! The CIE 1931 color space that many color spaces based on. */
	CIE_XYZ,

	/*! The CIE xyY color space. */
	CIE_xyY,

	/*! Linearized version of sRGB. */
	Linear_sRGB,

	/*! The good old sRGB color space. */
	sRGB,

	/*! A color space proposed by the AMPAS and they recommended it for rendering and compositing. */
	ACEScg,

	Spectral_Smits,

	// Special value for the number of entries (should always be the last one)
	NUM
};

}// end namespace ph::math
