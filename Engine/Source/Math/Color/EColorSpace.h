#pragma once

namespace ph::math
{

enum class EColorSpace
{
	UNSPECIFIED = 0,

	CIE_XYZ,
	Linear_sRGB,
	sRGB,
	Spectral_Smits,
	Spectral
};

}// end namespace ph::math
