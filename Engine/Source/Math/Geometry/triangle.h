#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Ray.h"

namespace ph
{

namespace math
{

bool is_intersecting_watertight_triangle(
	const Ray&      ray, 
	const Vector3R& vA, 
	const Vector3R& vB, 
	const Vector3R& vC, 
	real*           out_hitT,
	Vector3R*       out_hitBaryABCs);

template<typename T>
T triangle_area(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

}// end namespace math

}// end namespace ph

#include "Math/Geometry/triangle.ipp"
