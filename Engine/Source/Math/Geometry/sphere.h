#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Ray.h"

namespace ph::math::sphere
{

bool is_intersecting(
	const Ray&      ray,
	const Vector3R& center,
	real            radius,
	real*           out_hitT);

template<typename T>
T area(T radius);

// A common mapping method that is based on Archimedes' derivation that 
// the horizontal slices of a sphere have equal area.
template<typename T>
TVector3<T> uniform_unit_uv_to_position_archimedes(
	const TVector2<T>& uniformUnitUV, 
	T                  radius);

}// end namespace ph::math::sphere

#include "Math/Geometry/sphere.ipp"
