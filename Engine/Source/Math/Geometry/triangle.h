#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Ray.h"

namespace ph::math::triangle
{

bool is_intersecting_watertight(
	const Ray&      ray, 
	const Vector3R& vA, 
	const Vector3R& vB, 
	const Vector3R& vC, 
	real*           out_hitT,
	Vector3R*       out_hitBaryABCs);

template<typename T>
T area(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

template<typename T>
TVector3<T> position_to_barycentric(
	const TVector3<T>& position,
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

template<typename T>
TVector3<T> barycentric_to_position(
	const TVector3<T>& barycentricCoords,
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

// A common mapping on triangles which is based on slicing the triangular area
// into stripes.
// Reference: Osada et al., Section 4.2, "Shape Distributions", TOG02
template<typename T>
TVector3<T> uniform_unit_uv_to_barycentric_osada(
	const TVector2<T>& uniformUnitUV,
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

}// end namespace ph::math::triangle

#include "Math/Geometry/triangle.ipp"
