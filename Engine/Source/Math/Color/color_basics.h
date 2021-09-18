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

	// Standard illuminants
	A,
	B,
	C,
	D50,
	D55,
	D65,
	D75,
	E,
	F2,
	F7,
	F11
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

/*
Reference: http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
*/
template<typename T = ColorValue>
inline TTristimulusValues<T> CIEXYZ_of(const EReferenceWhite refWhite)
{
	switch(refWhite)
	{
	case EReferenceWhite::A:   return {1.09850, 1.00000, 0.35585};
	case EReferenceWhite::B:   return {0.99072, 1.00000, 0.85223};
	case EReferenceWhite::C:   return {0.98074, 1.00000, 1.18232};
	case EReferenceWhite::D50: return {0.96422, 1.00000, 0.82521};
	case EReferenceWhite::D55: return {0.95682, 1.00000, 0.92149};
	case EReferenceWhite::D65: return {0.95047, 1.00000, 1.08883};
	case EReferenceWhite::D75: return {0.94972, 1.00000, 1.22638};
	case EReferenceWhite::E:   return {1.00000, 1.00000, 1.00000};
	case EReferenceWhite::F2:  return {0.99186, 1.00000, 0.67393};
	case EReferenceWhite::F7:  return {0.95041, 1.00000, 1.08747};
	case EReferenceWhite::F11: return {1.00962, 1.00000, 0.64350};

	default: 
		PH_ASSERT_UNREACHABLE_SECTION();
		return {1.00000, 1.00000, 1.00000};
	}
}

template<typename T = ColorValue>
inline TChromaticityValues<T> chromaticity_of(const EReferenceWhite refWhite)
{
	const auto CIEXYZColor = CIEXYZ_of<T>(refWhite);

	// Reference whites should not have any component being <= 0
	PH_ASSERT_GT(CIEXYZColor[0], static_cast<T>(0));
	PH_ASSERT_GT(CIEXYZColor[1], static_cast<T>(0));
	PH_ASSERT_GT(CIEXYZColor[2], static_cast<T>(0));

	const T rcpSumXYZ = static_cast<T>(1) / (CIEXYZColor[0] + CIEXYZColor[1] + CIEXYZColor[2]);

	return
	{
		CIEXYZColor[0] * rcpSumXYZ,// x
		CIEXYZColor[1] * rcpSumXYZ // y
	};
}

}// end namespace ph::math
