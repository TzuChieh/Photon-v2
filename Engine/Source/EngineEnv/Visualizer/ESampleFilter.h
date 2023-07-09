#pragma once

namespace ph
{

enum class ESampleFilter
{
	Unspecified = 0,

	Box,
	Gaussian,
	MitchellNetravali,
	BlackmanHarris
};

}// end namespace ph
