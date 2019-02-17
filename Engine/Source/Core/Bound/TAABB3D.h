#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/math.h"

#include <vector>
#include <string>
#include <utility>

namespace ph
{

class Ray;

template<typename T>
class TAABB3D;

using AABB3D = TAABB3D<real>;

/*! @brief A 3-D Axis-Aligned Bounding Box.

A construct that have a cuboid-shaped boundary. An AABB, depending on the usage,
may represent a point, a layer of skin, or a volume.

@tparam T Arithmetic type used for the calculation.
*/
template<typename T>
class TAABB3D
{
public:
	/*! @brief Creates a new TAABB3D by unioning inputs.
	*/
	static TAABB3D makeUnioned(const TAABB3D& a, const TAABB3D& b);

public:
	/*! @brief Bounds nothing. State unspecified.
	*/
	TAABB3D();

	/*! @brief Bounds a point.
	*/
	TAABB3D(const TVector3<T>& point);

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

	/*! @brief Sets the coordinates of the corner with minumum values.
	*/
	void setMinVertex(const TVector3<T>& minVertex);

	/*! @brief Sets the coordinates of the corner with maxumum values.
	*/
	void setMaxVertex(const TVector3<T>& maxVertex);

	/*! @brief Makes the bounds grow by some amount.

	@param amount The amount to grow in 3 dimensions.
	*/
	void expand(const TVector3<T>& amount);

	/*! @brief Checks whether a ray intersects this volume.

	@return True if intersection is found, otherwise false.
	*/
	bool isIntersectingVolume(const Ray& ray) const;

	/*! @brief Checks whether a ray intersects this volume.

	@param[out] out_rayNearHitT Parametric distance where the ray starts to
	intersect this volume. 
	@param[out] out_rayFarHitT Parametric distance where the ray no longer
	intersects this volume. 
	@return True if intersection is found, otherwise false. When false is
	returned, the value of out_rayNearHitT and out_rayFarHitT is unspecified.
	*/
	bool isIntersectingVolume(
		const Ray& ray, 
		real*      out_rayNearHitT, 
		real*      out_rayFarHitT) const;

	/*! @brief Checks whether another bound intersects this volume.

	@param other The bound that is going to check against this volume.
	@return True if intersection is found, otherwise false.
	*/
	bool isIntersectingVolume(const TAABB3D& other) const;

	/*! @brief Checks whether this bound is a point.
	*/
	bool isPoint() const;

	/*! @brief Checks whether this bound encloses some amount of volume.
	*/
	bool isFiniteVolume() const;

	/*! @brief Get the 8 vertices of the bound.
	*/
	// FIXME: too slow
	std::vector<TVector3<T>> getVertices() const;

	/*! @brief Get corner vertices with the minimum and maximum values.
	*/
	void getMinMaxVertices(TVector3<T>* out_minVertex, TVector3<T>* out_maxVertex) const;

	/*! @brief Get the corner vertex with the minimum values.
	*/
	const TVector3<T>& getMinVertex() const;

	/*! @brief Get the corner vertex with the maximum values.
	*/
	const TVector3<T>& getMaxVertex() const;

	/*! @brief Get the center coordinates of the bound.
	*/
	Vector3R getCentroid() const;

	/*! @brief Get the side lengths of the bound.
	*/
	TVector3<T> getExtents() const;

	/*! @brief Get the surface area of the bound.
	*/
	T getSurfaceArea() const;

	/*! @brief Get the volume of the bound.
	*/
	T getVolume() const;

	/*! @brief Split the bound in specific position on an axis.

	@param axis The target axis.
	@param splitPoint The 1-D coordinate on the target axis.
	@return Two bounds which is the result of splitting this bound.
	*/
	// TODO: document possible values of axis
	std::pair<TAABB3D, TAABB3D> getSplitted(int axis, T splitPoint) const;

	/*! @brief Get the string representation of this bound.
	*/
	std::string toString() const;

	/*! @brief Checks whether this bound is equal to another bound.
	@return True if and only if two bounds are equal.
	*/
	// TODO: a variant with margins for floating types
	bool equals(const TAABB3D& other) const;

private:
	TVector3<T> m_minVertex;
	TVector3<T> m_maxVertex;
};

}// end namespace ph

#include "Core/Bound/TAABB3D.ipp"