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

using TristimulusValues    = TTristimulusValues<ColorValue>;
using SpectralSampleValues = TSpectralSampleValues<ColorValue>;

enum class EColorSpace
{
	UNSPECIFIED = 0,

	CIE_XYZ,
	Linear_sRGB,
	sRGB,
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

enum class EChromaticAdaptation
{
	UNSPECIFIED = 0,

	Bradford

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

}// end namespace ph::math
