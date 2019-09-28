#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Core/Ray.h"

#include <utility>

namespace ph::math::triangle
{

// TODO: templatize
bool is_intersecting_watertight(
	const Ray&      ray, 
	const Vector3R& vA, 
	const Vector3R& vB, 
	const Vector3R& vC, 
	real*           out_hitT,
	Vector3R*       out_hitBaryABCs);

template<typename T>
std::pair<TVector3<T>, TVector3<T>> edge_vectors(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

template<typename T>
T area(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

template<typename T>
TVector3<T> face_normal(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

// Calculates face normal. Note that the vertices may form a degenerate
// triangle (zero cross product and thus producing NaNs after being normalized,
// or vertices with Inf/NaN), in such case the failsafe vector will 
// be returned.
template<typename T>
TVector3<T> safe_face_normal(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC,
	const TVector3<T>& failSafe = {T(0), T(1), T(0)});

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

template<typename T>
bool is_degenerate(
	const TVector3<T>& vA,
	const TVector3<T>& vB,
	const TVector3<T>& vC);

}// end namespace ph::math::triangle

#include "Math/Geometry/triangle.ipp"
