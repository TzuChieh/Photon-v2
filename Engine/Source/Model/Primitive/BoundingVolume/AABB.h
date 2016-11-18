#pragma once

#include "Model/Primitive/BoundingVolume/BoundingVolume.h"
#include "Math/Vector3f.h"

namespace ph
{

class Ray;

class AABB final : public BoundingVolume
{
public:
	AABB(const Vector3f& minVertex, const Vector3f& maxVertex);
	virtual ~AABB() override;

	bool isIntersecting(const Ray& ray) const;

	inline void getMinMaxVertices(Vector3f* const out_minVertex, Vector3f* const out_maxVertex) const
	{
		out_minVertex->set(m_minVertex);
		out_maxVertex->set(m_maxVertex);
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