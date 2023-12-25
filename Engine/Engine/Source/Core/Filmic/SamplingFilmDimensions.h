#pragma once

#include "Math/TVector2.h"
#include "Math/Geometry/TAABB2D.h"

#include <Common/primitive_type.h>

namespace ph
{

struct SamplingFilmDimensions final
{
	math::TVector2<int64>  actualResPx;
	math::TAABB2D<int64>   effectiveWindowPx;
	math::TAABB2D<float64> sampleWindowPx;
};

}// end namespace ph
