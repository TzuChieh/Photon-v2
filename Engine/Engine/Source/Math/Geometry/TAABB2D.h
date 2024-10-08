#pragma once

#include "Math/TVector2.h"
#include "Math/constant.h"
#include "Math/hash.h"

#include <Common/primitive_type.h>
#include <Common/compiler.h>

#include <string>
#include <utility>
#include <array>
#include <functional>

namespace ph::math
{

template<typename T>
class TAABB2D;

using AABB2D = TAABB2D<real>;

/*! @brief A 2-D Axis-Aligned Bounding Box (AABB).
*/
template<typename T>
class TAABB2D final
{
public:
	/*! @brief Make an empty bound.
	Given a bound B, unioning B with an empty bound yields B itself, while interseting B with an empty
	bound is still an empty bound.
	*/
	static TAABB2D makeEmpty();

public:
	/*! @brief Unknown bounds. State unspecified.
	*/
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
	TAABB2D& setMinVertex(const TVector2<T>& minVertex);
	TAABB2D& setMaxVertex(const TVector2<T>& maxVertex);
	TAABB2D& setVertices(std::pair<TVector2<T>, TVector2<T>> minMaxVertices);
	const TVector2<T>& getMinVertex() const;
	const TVector2<T>& getMaxVertex() const;
	std::pair<TVector2<T>, TVector2<T>> getVertices() const;
	T getWidth() const;
	T getHeight() const;

	/*! @brief Get the side lengths of the bound.
	*/
	TVector2<T> getExtents() const;

	TVector2<T> getCenter() const;
	std::pair<TAABB2D, TAABB2D> getSplitted(std::size_t axis, T splitPoint) const;
	TAABB2D getUnioned(const TAABB2D& other) const;
	TAABB2D getIntersected(const TAABB2D& other) const;
	bool isEmpty() const;
	bool isPoint() const;
	bool isArea() const;

	/*! @brief Maps a 2D sample to the points within the AABB uniformly.
	*/
	TVector2<T> sampleToSurface(const std::array<T, 2>& sample) const;

	/*! @brief Maps a 2D vector in [0, 1] to the points within the AABB uniformly.
	*/
	TVector2<T> xy01ToSurface(const TVector2<T>& xy01) const;

	std::string toString() const;

	// TODO: a variant with margins for floating types
	bool isEqual(const TAABB2D& other) const;

	bool operator == (const TAABB2D& other) const;

#if !PH_COMPILER_HAS_P2468R2
	bool operator != (const TAABB2D& other) const;
#endif

private:
	TVector2<T> m_minVertex;
	TVector2<T> m_maxVertex;
};

}// end namespace ph::math

namespace std
{

template<typename T>
struct hash<ph::math::TAABB2D<T>>
{
	std::size_t operator () (const ph::math::TAABB2D<T>& aabb) const
	{
		return ph::math::murmur3_32(aabb, 0);
	}
};

}// end namespace std

#include "Math/Geometry/TAABB2D.ipp"
