#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Core/Bound/TAABB2D.h"

namespace ph
{

struct SamplingFilmDimensions
{
	TVector2<int64>  actualResPx;
	TAABB2D<int64>   effectiveWindowPx;
	TAABB2D<float64> sampleWindowPx;
};

}// end namespace ph