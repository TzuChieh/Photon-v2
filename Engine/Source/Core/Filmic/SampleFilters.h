#pragma once

#include "Core/Filmic/SampleFilter.h"

namespace ph
{

class SampleFilters final
{
public:
	static SampleFilter createBoxFilter();
	static SampleFilter createGaussianFilter();
	static SampleFilter createMitchellNetravaliFilter();
	static SampleFilter createBlackmanHarrisFilter();
};

}// end namespace ph