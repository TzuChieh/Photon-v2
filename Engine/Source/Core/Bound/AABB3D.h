#pragma once

#include "Math/TVector3.h"

#include <vector>
#include <string>

namespace ph
{

class Ray;

// TODO: 
// 1: templatize
// 2: add a split method (useful for kdtree)
class AABB3D final
{
public:
	static AABB3D makeUnioned(const AABB3D& a, const AABB3D& b);

public:
	AABB3D();
	AABB3D(const Vector3R& point);
	AABB3D(const Vector3R& minVertex, const Vector3R& maxVertex);

	bool isIntersectingVolume(const Ray& ray) const;
	bool isIntersectingVolume(const Ray& ray, real* out_rayNearHitT, real* out_rayFarHitT) const;
	bool isIntersectingVolume(const AABB3D& other) const;
	bool isPoint() const;
	bool isFiniteVolume() const;
	AABB3D& unionWith(const AABB3D& other);
	AABB3D& unionWith(const Vector3R& point);

	// FIXME: too slow
	std::vector<Vector3R> getVertices() const;

	void getMinMaxVertices(Vector3R* out_minVertex, Vector3R* out_maxVertex) const;
	const Vector3R& getMinVertex() const;
	const Vector3R& getMaxVertex() const;
	Vector3R calcCentroid() const;
	Vector3R calcExtents() const;
	real calcSurfaceArea() const;
	real calcVolume() const;
	void setMinVertex(const Vector3R& minVertex);
	void setMaxVertex(const Vector3R& maxVertex);
	void expand(const Vector3R& amount);

	std::string toString() const;

private:
	Vector3R m_minVertex;
	Vector3R m_maxVertex;
};

// In-header Implementations:

inline void AABB3D::getMinMaxVertices(Vector3R* const out_minVertex, Vector3R* const out_maxVertex) const
{
	PH_ASSERT(out_minVertex && out_maxVertex);

	out_minVertex->set(m_minVertex);
	out_maxVertex->set(m_maxVertex);
}

inline const Vector3R& AABB3D::getMinVertex() const
{
	return m_minVertex;
}

inline const Vector3R& AABB3D::getMaxVertex() const
{
	return m_maxVertex;
}

inline Vector3R AABB3D::calcCentroid() const
{
	return m_minVertex.add(m_maxVertex).mulLocal(0.5_r);
}

inline Vector3R AABB3D::calcExtents() const
{
	return m_maxVertex.sub(m_minVertex);
}

inline real AABB3D::calcSurfaceArea() const
{
	const Vector3R& extents = calcExtents();
	return 2.0_r * (extents.x * extents.y + extents.y * extents.z + extents.z * extents.x);
}

inline real AABB3D::calcVolume() const
{
	const Vector3R& extents = calcExtents();
	return extents.x * extents.y * extents.z;
}

inline void AABB3D::setMinVertex(const Vector3R& minVertex)
{
	m_minVertex = minVertex;
}

inline void AABB3D::setMaxVertex(const Vector3R& maxVertex)
{
	m_maxVertex = maxVertex;
}

inline void AABB3D::expand(const Vector3R& amount)
{
	m_minVertex.subLocal(amount);
	m_maxVertex.addLocal(amount);
}

inline bool AABB3D::isPoint() const
{
	return m_minVertex.equals(m_maxVertex);
}

inline bool AABB3D::isFiniteVolume() const
{
	return std::isnormal(calcVolume());
}

}// end namespace ph