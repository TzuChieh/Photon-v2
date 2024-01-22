#pragma once

namespace ph::math
{

enum class EColorSpace
{
	/*
	Update `is_tristimulus()` if entries are modified.
	*/

	Unspecified = 0,

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

	/*! Using spectral values as a color space, and performing down-sample (e.g., spectral to tristimulus)
	and up-sample (e.g., tristimulus to spectral) operations with Smit's method.
	*/
	Spectral_Smits,

	// Special values:
	
	/*! Number of entries (should always be the one next to last entry). */
	NUM,

	/*! Default spectral representation. Also for general spectral representation that does not want
	to be tied to a specific spectral color space.
	*/
	Spectral = Spectral_Smits
};

enum class EReferenceWhite
{
	Unspecified = 0,

	// CIE Illuminant A
	A,

	// CIE Illuminant B
	B,

	// CIE Illuminant C
	C,

	// Illuminant D Series: represent natural light
	D50,
	D55,
	D60,
	D65,
	D75,

	// Equal-energy radiator
	E,

	// Illuminant F Series: various types of fluorescent lighting
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,

	// Illuminant Series LED: different LED types (approx. 2700 K to 6600 K)
	LED_B1,
	LED_B2,
	LED_B3,
	LED_B4,
	LED_B5,
	LED_BH1,
	LED_RGB1,
	LED_V1,
	LED_V2,

	// Custom White Points

	/*! 
	The Academy Color Encoding System (ACES) white point, it is close to D60 
	(but not D60! many implementations got this wrong). 
	*/
	ACES,

	// Special values:

	// Number of entries (should always be the one next to last entry)
	NUM
};

/*! @brief Methods to map image appearance between different illumination sources.
This kind of mapping is commonly called Chromatic Adaptation Transform (CAT).
*/
enum class EChromaticAdaptation
{
	Unspecified = 0,

	/*! The most simple transform. Generally considered to be an inferior CAT. */
	XYZScaling,

	/*! Adobe uses this CAT in all of their products according to many sources. */
	Bradford,

	VonKries,

	// TODO: https://en.wikipedia.org/wiki/CIECAM02
	//CAT02

	// TODO: spectral route

	// Special values:

	Default = Bradford
};

/*!
These can be considered as hints that can be specified while converting data between color spaces. 
Specifying a hint to a method may result in better-converted data depending on the implementation.
*/
enum class EColorUsage
{
	Unspecified = 0,

	/*!
	Raw numeric data such as position, normal, density information.
	*/
	RAW,

	/*!
	EMR stands for ElectroMagnetic Radiation, used by energy emitters such as light sources.
	*/
	EMR,

	/*!
	ECF stands for Energy Conservative Fraction; surface albedo, reflectance, transmittance... are 
	all ECFs. It is required that an ECF have value within [0, 1].
	*/
	ECF
};

enum class ESpectralResample
{
	Unspecified = 0,

	PiecewiseAveraged,

	// TODO: gaussian and other methods

	// Special values:

	Default = PiecewiseAveraged
};

}// end namespace ph::math
