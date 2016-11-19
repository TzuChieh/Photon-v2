#pragma once

#include "Model/Primitive/BoundingVolume/BoundingVolume.h"
#include "Math/Vector3f.h"

namespace ph
{

class Ray;

class AABB final : public BoundingVolume
{
public:
	AABB();
	AABB(const Vector3f& minVertex, const Vector3f& maxVertex);
	virtual ~AABB() override;

	bool isIntersecting(const Ray& ray) const;
	bool isIntersecting(const Ray& ray, float32* const out_rayNearHitDist, float32* const out_rayFarHitDist) const;
	bool isIntersecting(const AABB& aabb) const;
	void unionWith(const AABB& other);

	inline void getMinMaxVertices(Vector3f* const out_minVertex, Vector3f* const out_maxVertex) const
	{
		out_minVertex->set(m_minVertex);
		out_maxVertex->set(m_maxVertex);
	}

	inline const Vector3f& getMinVertex() const
	{
		return m_minVertex;
	}

	inline const Vector3f& getMaxVertex() const
	{
		return m_maxVertex;
	}

	inline void setMinVertex(const Vector3f& minVertex)
	{
		m_minVertex = minVertex;
	}

	inline void setMaxVertex(const Vector3f& maxVertex)
	{
		m_maxVertex = maxVertex;
	}

private:
	Vector3f m_minVertex;
	Vector3f m_maxVertex;
};

}// end namespace ph