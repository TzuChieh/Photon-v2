#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <string>

namespace ph
{

template<typename T>
class TAABB2D;

using AABB2D = TAABB2D<real>;

template<typename T>
class TAABB2D
{
public:
	TVector2<T> minVertex;
	TVector2<T> maxVertex;

	TAABB2D();
	TAABB2D(const TAABB2D& other);
	TAABB2D(const TVector2<T>& minVertex, const TVector2<T>& maxVertex);

	template<typename U>
	explicit TAABB2D(const TAABB2D<U>& other);

	bool isIntersectingArea(const TAABB2D& other) const;
	bool isIntersectingArea(const TVector2<T>& point) const;
	T calcArea() const;
	TAABB2D& unionWith(const TAABB2D& other);
	TAABB2D& intersectWith(const TAABB2D& other);
	T getWidth() const;
	T getHeight() const;
	TVector2<T> getExtents() const;
	TVector2<T> calcCenter() const;
	bool isValid() const;
	bool isPoint() const;
	bool isArea() const;
	bool equals(const TAABB2D& other) const;
	std::string toString() const;
};

}// end namespace ph

#include "Core/Bound/TAABB2D.ipp"