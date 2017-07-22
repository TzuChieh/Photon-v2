#pragma once

#include "Core/Filmic/SampleFilter.h"

#include <string>

namespace ph
{

class SampleFilterFactory final
{
public:
	static SampleFilter createBoxFilter();
	static SampleFilter createGaussianFilter();
};

}// end namespace ph