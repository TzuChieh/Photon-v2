#pragma once

#include "Engine/Math/Geometry/TBasicTriangle.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Geometry/TLineSegment.h"

#include <Common/compiler.h>
#include <Common/primitive_type.h>

namespace ph::math
{

template<typename T>
class TWatertightTriangle final : public TBasicTriangle<T>
{
public:
	using TBasicTriangle<T>::TBasicTriangle;

	/*! @brief Checks whether the `segment` is interseting with this triangle.
	@param segment The line section to intersect with.
	@param[out] out_hitT The parametric distance of the hit point for `segment`.
	@param[out] out_hitBarycentricCoords The barycentric coordinates of the hit point for this triangle.
	@note It is advisible to use `out_hitBarycentricCoodrs` to calculate the hit point as the
	numerical error will generally be smaller than using `out_hitT`.
	*/
	[[PH_ALWAYS_INLINE]]
	bool isIntersecting(
		const TLineSegment<T>& segment,
		T*                     out_hitT,
		TVector3<T>*           out_hitBarycentricCoords) const;

	[[PH_ALWAYS_INLINE]]
	bool isIntersecting(const TLineSegment<T>& segment) const;

	/*! @brief Same as `isIntersecting()`, except the cost is slightly higher to reduce numerical error.
	*/
	/*bool isIntersectingRefined(
		const TLineSegment<T>& segment,
		T*                     out_hitT,
		TVector3<T>*           out_hitBarycentricCoords) const;*/
};

}// end namespace ph::math

#include "Engine/Math/Geometry/TWatertightTriangle.ipp"
