#pragma once

#include "Math/Geometry/TBasicTriangle.h"
#include "Math/math_fwd.h"
#include "Math/Geometry/TLineSegment.h"

#include <Common/primitive_type.h>

namespace ph::math
{

template<typename T>
class TWatertightTriangle final : public TBasicTriangle<T>
{
public:
	using TBasicTriangle<T>::TBasicTriangle;

	bool isIntersecting(
		const TLineSegment<T>& segment,
		T*                     out_hitT,
		TVector3<T>*           out_hitBarycentricCoords) const;
};

}// end namespace ph::math

#include "Math/Geometry/TWatertightTriangle.ipp"
