#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Ray.h"

namespace ph
{

namespace math
{

bool is_intersecting_sphere(
	const Ray&      ray,
	const Vector3R& center,
	real            radius,
	real*           out_hitT);

}// end namespace math

}// end namespace ph

#include "Math/Geometry/sphere.ipp"
