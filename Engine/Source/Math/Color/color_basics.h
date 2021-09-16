#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"

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
template<EReferenceWhite REFERENCE_WHITE, typename T = ColorValue>
inline constexpr TTristimulusValues<T> CIEXYZ_of() noexcept
{
	static_assert(REFERENCE_WHITE != EReferenceWhite::UNSPECIFIED,
		"Please provide a valid refernece white.");

	if constexpr(REFERENCE_WHITE == EReferenceWhite::A)
	{
		return {1.09850, 1.00000, 0.35585};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::B)
	{
		return {0.99072, 1.00000, 0.85223};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::C)
	{
		return {0.98074, 1.00000, 1.18232};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::D50)
	{
		return {0.96422, 1.00000, 0.82521};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::D55)
	{
		return {0.95682, 1.00000, 0.92149};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::D65)
	{
		return {0.95047, 1.00000, 1.08883};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::D75)
	{
		return {0.94972, 1.00000, 1.22638};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::E)
	{
		return {1.00000, 1.00000, 1.00000};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::F2)
	{
		return {0.99186, 1.00000, 0.67393};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::F7)
	{
		return {0.95041, 1.00000, 1.08747};
	}
	else if constexpr(REFERENCE_WHITE == EReferenceWhite::F11)
	{
		return {1.00962, 1.00000, 0.64350};
	}
	else
	{
		static_assert(REFERENCE_WHITE == EReferenceWhite::UNSPECIFIED,
			"Not all refernece whites have a return value.");

		return {0.00000, 0.00000, 0.00000};
	}
}

}// end namespace ph::math
