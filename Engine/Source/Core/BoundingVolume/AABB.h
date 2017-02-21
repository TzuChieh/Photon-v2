#pragma once

#include "Math/TVector3.h"

namespace ph
{

class Ray;

class AABB final
{
public:
	static AABB makeUnioned(const AABB& a, const AABB& b);

public:
	AABB();
	AABB(const Vector3R& point);
	AABB(const Vector3R& minVertex, const Vector3R& maxVertex);

	bool isIntersectingVolume(const Ray& ray) const;
	bool isIntersectingVolume(const Ray& ray, real* const out_rayNearHitT, real* const out_rayFarHitT) const;
	bool isIntersectingVolume(const AABB& aabb) const;
	bool isPoint() const;
	AABB& unionWith(const AABB& other);
	AABB& unionWith(const Vector3R& point);

	inline void getMinMaxVertices(Vector3R* const out_minVertex, Vector3R* const out_maxVertex) const
	{
		out_minVertex->set(m_minVertex);
		out_maxVertex->set(m_maxVertex);
	}

	inline const Vector3R& getMinVertex() const
	{
		return m_minVertex;
	}

	inline const Vector3R& getMaxVertex() const
	{
		return m_maxVertex;
	}

	inline Vector3R calcCentroid() const
	{
		return m_minVertex.add(m_maxVertex).mulLocal(0.5_r);
	}

	inline Vector3R calcExtents() const
	{
		return m_maxVertex.sub(m_minVertex);
	}

	inline real calcSurfaceArea() const
	{
		const Vector3R& extents = calcExtents();
		return 2.0_r * (extents.x * extents.y + extents.y * extents.z + extents.z * extents.x);
	}

	inline void setMinVertex(const Vector3R& minVertex)
	{
		m_minVertex = minVertex;
	}

	inline void setMaxVertex(const Vector3R& maxVertex)
	{
		m_maxVertex = maxVertex;
	}

private:
	Vector3R m_minVertex;
	Vector3R m_maxVertex;
};

}// end namespace ph