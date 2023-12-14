#pragma once

#include "Math/TVector3.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Math/Geometry/TLineSegment.h"

#include <Common/primitive_type.h>

#include <array>
#include <string>
#include <utility>

namespace ph::math
{

template<typename T>
class TAABB3D;

using AABB3D = TAABB3D<real>;

/*! @brief A 3-D Axis-Aligned Bounding Box.

A construct that have a cuboid-shaped boundary. An AABB, depending on the usage,
may represent a point, a layer of skin, or a volume.

@tparam T Arithmetic type used for the calculation.
*/
template<typename T>
class TAABB3D final
{
public:
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

	@return True if intersection is found, otherwise false.
	*/
	bool isIntersectingVolume(const TLineSegment<T>& segment) const;

	/*! @brief Checks whether a line segment intersects this volume.

	Returned boolean value indicates whether the ray is intersecting with the
	AABB's volume or not. If there's an intersection, the near and far hit
	distances will be returned via (out_rayNearHitDist, out_rayFarHitDist); if
	the starting point of the ray is inside the AABB, near hit distance will be
	the ray's t_min since volume intersection starts immediately on the ray's
	starting point (the ending point of the ray follows the same logic).

	@param[out] out_nearHitT Parametric distance where the line segment starts
	to intersect this volume. 
	@param[out] out_farHitT Parametric distance where the line segment no 
	longer intersects this volume. 
	@return True if intersection is found, otherwise false. When false is
	returned, the value of out_nearHitT and out_farHitT is unspecified.
	*/
	bool isIntersectingVolume(
		const TLineSegment<T>& segment,
		T* out_nearHitT, 
		T* out_farHitT) const;

	/*! @brief Checks whether another bound intersects this volume.

	@param other The bound that is going to check against this volume.
	@return True if intersection is found, otherwise false.
	*/
	bool isIntersectingVolume(const TAABB3D& other) const;

	bool isEmpty() const;

	/*! @brief Checks whether this bound is a point.
	*/
	bool isPoint() const;

	/*! @brief Checks whether this bound encloses some amount of volume.
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
	std::pair<TAABB3D, TAABB3D> getSplitted(constant::AxisIndexType axis, T splitPoint) const;

	/*! @brief Get the string representation of this bound.
	*/
	std::string toString() const;

	/*! @brief Checks whether this bound is equal to another bound.
	@return True if and only if two bounds are equal.
	*/
	// TODO: a variant with margins for floating types
	bool isEqual(const TAABB3D& other) const;

private:
	/*!
	Reference: Kay and Kayjia's "slab method" from a project of the ACM
	SIGGRAPH Education Committee named HyperGraph.
	*/
	bool isIntersectingVolumeKajiyaKay(
		const TLineSegment<T>& segment,
		T* out_nearHitT,
		T* out_farHitT) const;

	/*!
	A fast and branchless method developed by Tavian Barnes. The algorithm is
	based on slab method.
	*/
	bool isIntersectingVolumeTavian(
		const TLineSegment<T>& segment,
		T* out_nearHitT,
		T* out_farHitT) const;

	TVector3<T> m_minVertex;
	TVector3<T> m_maxVertex;
};

}// end namespace ph::math

#include "Math/Geometry/TAABB3D.ipp"
