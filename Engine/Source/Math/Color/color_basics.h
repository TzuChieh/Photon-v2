#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"
#include "Common/assertion.h"

#include <array>

namespace ph::math
{

using ColorValue = real;

template<typename T>
using TTristimulusValues = std::array<T, 3>;

template<typename T>
using TSpectralSampleValues = std::array<T, PH_SPECTRUM_SAMPLED_NUM_SAMPLES>;

template<typename T>
using TChromaticityValues = std::array<T, 2>;

using TristimulusValues    = TTristimulusValues<ColorValue>;
using SpectralSampleValues = TSpectralSampleValues<ColorValue>;
using ChromaticityValues   = TChromaticityValues<ColorValue>;

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
	Spectral
};

enum class EReferenceWhite
{
	UNSPECIFIED = 0,

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

	// Eual-energy radiator
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
	ACES
};

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

enum class EColorUsage
{
	UNSPECIFIED = 0,

	RAW,
	EMR,
	ECF
};

/*!
Values are for 10-degree observer.

References: 
[1] https://en.wikipedia.org/wiki/Standard_illuminant

The source code of Colour python library:
[2] https://github.com/colour-science/colour/blob/v0.3.16/colour/colorimetry/datasets/illuminants/chromaticity_coordinates.py
*/
template<typename T = ColorValue>
inline TChromaticityValues<T> chromaticity_of(const EReferenceWhite refWhite)
{
	switch(refWhite)
	{
	case EReferenceWhite::A:        return {0.45117, 0.40594};
	case EReferenceWhite::B:        return {0.34980, 0.35270};
	case EReferenceWhite::C:        return {0.31039, 0.31905};
	case EReferenceWhite::D50:      return {0.34773, 0.35952};
	case EReferenceWhite::D55:      return {0.33411, 0.34877};
	case EReferenceWhite::D60:      return {0.322986926715820, 0.339275732345997};
	case EReferenceWhite::D65:      return {0.31382, 0.33100};
	case EReferenceWhite::D75:      return {0.29968, 0.31740};
	case EReferenceWhite::E:        return {1.0 / 3.0, 1.0 / 3.0};
	case EReferenceWhite::F1:       return {0.31811, 0.33559};
	case EReferenceWhite::F2:       return {0.37925, 0.36733};
	case EReferenceWhite::F3:       return {0.41761, 0.38324};
	case EReferenceWhite::F4:       return {0.44920, 0.39074};
	case EReferenceWhite::F5:       return {0.31975, 0.34246};
	case EReferenceWhite::F6:       return {0.38660, 0.37847};
	case EReferenceWhite::F7:       return {0.31569, 0.32960};
	case EReferenceWhite::F8:       return {0.34902, 0.35939};
	case EReferenceWhite::F9:       return {0.37829, 0.37045};
	case EReferenceWhite::F10:      return {0.35090, 0.35444};
	case EReferenceWhite::F11:      return {0.38541, 0.37123};
	case EReferenceWhite::F12:      return {0.44256, 0.39717};
	case EReferenceWhite::LED_B1:   return {0.462504966271043, 0.403041801546906};
	case EReferenceWhite::LED_B2:   return {0.442119475258745, 0.396633702892576};
	case EReferenceWhite::LED_B3:   return {0.380851979328052, 0.368518548904765};
	case EReferenceWhite::LED_B4:   return {0.348371362473402, 0.345065503264192};
	case EReferenceWhite::LED_B5:   return {0.316916877024753, 0.322060276350364};
	case EReferenceWhite::LED_BH1:  return {0.452772610754910, 0.400032462750000};
	case EReferenceWhite::LED_RGB1: return {0.457036370583652, 0.425381348780888};
	case EReferenceWhite::LED_V1:   return {0.453602699414564, 0.398199587905174};
	case EReferenceWhite::LED_V2:   return {0.377728483834020, 0.374512315539769};

	// References: 
	// [1] TB-2014-004: Informative Notes on SMPTE ST 2065-1 ¡V Academy Color Encoding Specification (ACES)
	// https://www.oscars.org/science-technology/aces/aces-documentation
	// [2] TB-2018-001: Derivation of the ACES White Point Chromaticity Coordinates
	// https://www.oscars.org/science-technology/aces/aces-documentation
	case EReferenceWhite::ACES: return {0.32168, 0.33767};

	default: 
		PH_ASSERT_UNREACHABLE_SECTION();
		return {1.0 / 3.0, 1.0 / 3.0};// Same as standard illuminant E
	}
}

/*
For reference whites, set Y value to 1 (the brightest white that a color display supports).
*/
template<typename T = ColorValue>
inline TTristimulusValues<T> CIEXYZ_of_custom_reference_white(const TChromaticityValues<T>& refWhite)
{
	// Reference whites should not have any component being <= 0
	PH_ASSERT_GT(refWhite[0], static_cast<T>(0));
	PH_ASSERT_GT(refWhite[1], static_cast<T>(0));

	const T x = refWhite[0];
	const T y = refWhite[1];

	// Y is set to 1
	const T Y_over_y = static_cast<T>(1) / y;
	return
	{
		Y_over_y * x,
		1.0,
		Y_over_y * (static_cast<T>(1) - x - y)
	};
}

template<typename T = ColorValue>
inline TTristimulusValues<T> CIEXYZ_of(const EReferenceWhite refWhite)
{
	const auto chromaticity = chromaticity_of<T>(refWhite);
	return CIEXYZ_of_custom_reference_white<T>(chromaticity);
}

}// end namespace ph::math
