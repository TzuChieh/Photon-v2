#include "Model/Primitive/BoundingVolume/AABB.h"
#include "Core/Ray.h"

namespace ph
{

AABB::AABB(const Vector3f& minVertex, const Vector3f& maxVertex) : 
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{

}

AABB::~AABB() = default;

bool AABB::isIntersecting(const Ray& ray) const
{
	// Reference: Kay and Kayjia's "slab method" from a project of the ACM SIGGRAPH Education Committee
	// named HyperGraph.
	// Note: This algorithm can produce NaNs which will generate false positives in rare cases. Although 
	// it can be handled, we ignore it since the performance will degrade, also bounding volumes are allowed 
	// to produce false positives already.

	float32 tMin, tMax;

	const float32 txMin = (m_minVertex.x - ray.getOrigin().x) / ray.getDirection().x;
	const float32 txMax = (m_maxVertex.x - ray.getOrigin().x) / ray.getDirection().x;
	if(txMin < txMax)
	{
		tMin = txMin;
		tMax = txMax;
	}
	else
	{
		tMin = txMax;
		tMax = txMin;
	}

	const float32 tyMin = (m_minVertex.y - ray.getOrigin().y) / ray.getDirection().y;
	const float32 tyMax = (m_maxVertex.y - ray.getOrigin().y) / ray.getDirection().y;
	if(tyMin < tyMax)
	{
		tMin = tMin > tyMin ? tMin : tyMin;
		tMax = tMax > tyMax ? tyMax : tMax;
	}
	else
	{
		tMin = tMin > tyMax ? tMin : tyMax;
		tMax = tMax > tyMin ? tyMin : tMax;
	}

	const float32 tzMin = (m_minVertex.z - ray.getOrigin().z) / ray.getDirection().z;
	const float32 tzMax = (m_maxVertex.z - ray.getOrigin().z) / ray.getDirection().z;
	if(tzMin < tzMax)
	{
		tMin = tMin > tzMin ? tMin : tzMin;
		tMax = tMax > tzMax ? tzMax : tMax;
	}
	else
	{
		tMin = tMin > tzMax ? tMin : tzMax;
		tMax = tMax > tzMin ? tzMin : tMax;
	}

	return (tMax >= 0.0f) && (tMax >= tMin);
}

}// end namespace ph