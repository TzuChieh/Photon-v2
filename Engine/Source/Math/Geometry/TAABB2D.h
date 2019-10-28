#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/constant.h"

#include <string>
#include <utility>

namespace ph::math
{

template<typename T>
class TAABB2D;

using AABB2D = TAABB2D<real>;

template<typename T>
class TAABB2D final
{
public:
	// TODO: make these private or?
	TVector2<T> minVertex;
	TVector2<T> maxVertex;

	TAABB2D();
	TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex);
	explicit TAABB2D(const TVector2<T>& point);

	template<typename U>
	explicit TAABB2D(const TAABB2D<U>& other);

	bool isIntersectingArea(const TAABB2D& other) const;
	bool isIntersectingArea(const TVector2<T>& point) const;
	bool isIntersectingRange(const TVector2<T>& point) const;
	T getArea() const;
	TAABB2D& unionWith(const TAABB2D& other);
	TAABB2D& intersectWith(const TAABB2D& other);
	T getWidth() const;
	T getHeight() const;
	TVector2<T> getExtents() const;
	TVector2<T> getCenter() const;
	std::pair<TAABB2D, TAABB2D> getSplitted(constant::AxisIndexType axis, T splitPoint) const;
	TAABB2D getUnioned(const TAABB2D& other) const;
	TAABB2D getIntersected(const TAABB2D& other) const;
	bool isValid() const;
	bool isPoint() const;
	bool isArea() const;

	std::string toString() const;

	// TODO: a variant with margins for floating types
	bool equals(const TAABB2D& other) const;
};

}// end namespace ph::math

#include "Math/Geometry/TAABB2D.ipp"
