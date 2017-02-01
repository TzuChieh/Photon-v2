#pragma once

#include "Math/TVector3.h"

namespace ph
{

class Ray;

class AABB final
{
public:
	AABB();
	AABB(const Vector3R& minVertex, const Vector3R& maxVertex);

	bool isIntersectingVolume(const Ray& ray) const;
	bool isIntersectingVolume(const Ray& ray, real* const out_rayNearHitDist, real* const out_rayFarHitDist) const;
	bool isIntersectingVolume(const AABB& aabb) const;
	void unionWith(const AABB& other);

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