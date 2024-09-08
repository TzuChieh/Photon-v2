#pragma once

#include "Math/TVector3.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Math/Geometry/TLineSegment.h"

#include <Common/primitive_type.h>
#include <Common/compiler.h>

#include <array>
#include <string>
#include <utility>

namespace ph::math
{

template<typename T>
class TAABB3D;

using AABB3D = TAABB3D<real>;

/*! @brief A 3-D Axis-Aligned Bounding Box (AABB).

A construct that have a cuboid-shaped boundary. An AABB, depending on the usage,
may represent a point, a layer of skin, or a volume.

@tparam T Arithmetic type used for the calculation.
*/
template<typename T>
class TAABB3D final
{
public:
	/*! @brief Make an empty bound.
	Given a bound B, unioning B with an empty bound yields B itself, while interseting B with an empty
	bound is still an empty bound.
	*/
	static TAABB3D makeEmpty();

	/*! @brief Creates a new TAABB3D by unioning inputs.
	*/
	static TAABB3D makeUnioned(const TAABB3D& a, const TAABB3D& b);

public:
	/*! @brief Unknown bounds. State unspecified.
	*/
	TAABB3D();

	/*! @brief Bounds a point.
	*/
	explicit TAABB3D(const TVector3<T>& point);

	/*! @brief Bounds a volume.

	@param minVertex Coordinates of the corner with minimum values.
	@param maxVertex Coordinates of the corner with maximum values.
	*/
	TAABB3D(const TVector3<T>& minVertex, const TVector3<T>& maxVertex);

	/*! @brief Unions this bound with another one.

	@return This bound after being unioned.
	*/
	TAABB3D& unionWith(const TAABB3D& other);

	/*! @brief Unions this TAABB3D with a point.

	@return This bound after being unioned.
	*/
	TAABB3D& unionWith(const TVector3<T>& point);

	/*! @brief Set the corner vertex of the maximum (+++) octant.
	*/
	TAABB3D& setMinVertex(const TVector3<T>& minVertex);

	/*! @brief Set the corner vertex of the minimum (---) octant.
	*/
	TAABB3D& setMaxVertex(const TVector3<T>& maxVertex);

	/*! @brief Set the corner vertices of the minimum (---) and maximum (+++) octants.
	*/
	TAABB3D& setVertices(std::pair<TVector3<T>, TVector3<T>> minMaxVertices);

	TAABB3D& translate(const TVector3<T>& amount);

	/*! @brief Makes the bounds grow by some amount.

	@param amount The amount to grow in 3 dimensions.
	*/
	TAABB3D& expand(const TVector3<T>& amount);

	/*! @brief Checks whether a line segment intersects this volume.
	@param segment The line section to intersect with.
	@return `true` if intersection is found, `false` otherwise.
	*/
	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	bool isIntersectingVolume(const TLineSegment<T>& segment) const;

	/*! @brief Checks whether a line segment intersects this volume.

	Returned boolean value indicates whether the ray is intersecting with the
	AABB's volume or not. If there's an intersection, the near and far hit
	distances will be returned via (out_rayNearHitDist, out_rayFarHitDist); if
	the starting point of the ray is inside the AABB, near hit distance will be
	the ray's t_min since volume intersection starts immediately on the ray's
	starting point (the ending point of the ray follows the same logic).

	@tparam IS_ROBUST Whether to perform a robust intersection test. A robust test is slightly slower,
	but handles more edge cases and is more conservative (return `true` on degenerate configurations).
	@param segment The segment to test against this volume.
	@param[out] out_nearHitT Parametric distance where the line segment starts
	to intersect this volume. 
	@param[out] out_farHitT Parametric distance where the line segment no 
	longer intersects this volume. 
	@return `true` if intersection is found, otherwise `false`. When `false` is
	returned, the value of `out_nearHitT` and `out_farHitT` is unspecified.
	*/
	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	bool isIntersectingVolume(
		const TLineSegment<T>& segment,
		T* out_nearHitT, 
		T* out_farHitT) const;

	/*!
	Variant of `isIntersectingVolume()` that can benefit from a precalculated reciprocal of
	segment direction `rcpSegmentDir`.
	*/
	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	bool isIntersectingVolume(
		const TLineSegment<T>& segment,
		const TVector3<T>& rcpSegmentDir,
		T* out_nearHitT, 
		T* out_farHitT) const;

	/*!
	Variant of `isIntersectingVolume()` that returns the near and far hit distances rather
	than returning a boolean. This can potentially remove a branch if the returned distances
	are all the caller needs.
	@return Near and far parametric hit distances. The `segment` is intersecting this volume if
	@f$ t_{near} <= t_{far} @f$.
	*/
	template<bool IS_ROBUST = true>
	[[PH_ALWAYS_INLINE]]
	std::pair<T, T> isIntersectingVolume(
		const TLineSegment<T>& segment,
		const TVector3<T>& rcpSegmentDir) const;

	/*! @brief Checks whether another bound intersects this volume.

	@param other The bound that is going to check against this volume.
	@return `true` if intersection is found, `false` otherwise.
	*/
	bool isIntersectingVolume(const TAABB3D& other) const;

	/*! @brief Checks whether this bound encloses nothing, not even a point.
	*/
	bool isEmpty() const;

	/*! @brief Checks whether this bound is a point.
	*/
	bool isPoint() const;

	/*! @brief Checks whether this bound encloses some amount of volume.
	*/
	bool isVolume() const;

	/*! @brief Checks whether this bound encloses some finite amount of volume.
	This is a stricter test than `isVolume()`. This bound must have a finite volume for this test to
	return `true`.
	*/
	bool isFiniteVolume() const;

	/*! @brief Get corner vertices on the minimum (---) and maximum (+++) octants.

	@return A pair of vertices in the form <min-vertex, max-vertex>.
	*/
	std::pair<TVector3<T>, TVector3<T>> getVertices() const;

	/*! @brief Get the 8 vertices of the bound.
	*/
	std::array<TVector3<T>, 8> getBoundVertices() const;

	/*! @brief Get the corner vertex of the minimum (---) octant.
	*/
	const TVector3<T>& getMinVertex() const;

	/*! @brief Get the corner vertex of the maximum (+++) octant.
	*/
	const TVector3<T>& getMaxVertex() const;

	/*! @brief Get the center coordinates of the bound.
	*/
	TVector3<T> getCentroid() const;

	/*! @brief Get the side lengths of the bound.
	*/
	TVector3<T> getExtents() const;

	/*! @brief Get the surface area of the bound.
	*/
	T getSurfaceArea() const;

	/*! @brief Get the volume of the bound.
	*/
	T getVolume() const;

	TAABB3D getTranslated(const TVector3<T>& amount) const;

	/*! @brief Split the bound in specific position on an axis.

	@param axis The target axis.
	@param splitPoint The 1-D coordinate on the target axis.
	@return Two bounds which is the result of splitting this bound.
	*/
	std::pair<TAABB3D, TAABB3D> getSplitted(std::size_t axis, T splitPoint) const;

	/*! @brief Get the string representation of this bound.
	*/
	std::string toString() const;

	/*! @brief Checks whether this bound is equal to another bound.
	@return True if and only if two bounds are equal.
	*/
	bool isEqual(const TAABB3D& other) const;
	// TODO: a variant with margins for floating types

private:
	/*!
	Reference: Kay and Kayjia's "slab method" from a project of the ACM
	SIGGRAPH Education Committee named HyperGraph.
	@return Near and far parametric hit distances. The `segment` is intersecting this volume if
	@f$ t_{near} <= t_{far} @f$.
	*/
	[[PH_ALWAYS_INLINE]]
	std::pair<T, T> intersectVolumeKajiyaKay(
		const TLineSegment<T>& segment,
		const TVector3<T>& rcpSegmentDir) const;

	/*!
	A fast and branchless method developed by Tavian Barnes. The algorithm is
	based on slab method.
	@return Near and far parametric hit distances. The `segment` is intersecting this volume if
	@f$ t_{near} <= t_{far} @f$.
	*/
	[[PH_ALWAYS_INLINE]]
	std::pair<T, T> intersectVolumeTavian(
		const TLineSegment<T>& segment,
		const TVector3<T>& rcpSegmentDir) const;

	/*!
	Robust AABB intersection routine from the paper by Thiago Ize @cite Ize:2013:Robust from Solid
	Angle (develops the Arnold renderer). This method implements the "MaxMult" algorithm described
	in the paper, with slight modifications to adapt to our rendering system.
	@return Near and far parametric hit distances. The `segment` is intersecting this volume if
	@f$ t_{near} <= t_{far} @f$.
	*/
	[[PH_ALWAYS_INLINE]]
	std::pair<T, T> intersectVolumeRobust(
		const TLineSegment<T>& segment,
		const TVector3<T>& rcpSegmentDir) const;

	TVector3<T> m_minVertex;
	TVector3<T> m_maxVertex;
};

}// end namespace ph::math

#include "Math/Geometry/TAABB3D.ipp"
