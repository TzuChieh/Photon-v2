#pragma once

#include "Common/primitive_type.h"
#include "Math/Geometry/TBasicTriangle.h"
#include "Math/math_fwd.h"
#include "Math/Geometry/TLineSegment.h"

namespace ph::math
{

template<typename T>
class TTriangle final : public TBasicTriangle<T>
{
public:
	using TBasicTriangle<T>::TBasicTriangle;

	// TODO: basic intersection
};

}// end namespace ph::math
