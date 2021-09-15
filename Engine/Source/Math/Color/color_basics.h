#pragma once

#include "Common/primitive_type.h"
#include "Common/config.h"

#include <array>

namespace ph::math
{

using ColorValue           = real;
using TristimulusValues    = std::array<ColorValue, 3>;
using SpectralSampleValues = std::array<ColorValue, PH_SPECTRUM_SAMPLED_NUM_SAMPLES>;

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

	D65,
	E,
	D50
};

enum class EChromaticAdaption
{
	UNSPECIFIED = 0,

	Bradford
};

enum class EColorUsage
{
	UNSPECIFIED = 0,

	RAW,
	EMR,
	ECF
};

}// end namespace ph::math
