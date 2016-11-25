#include "Model/BoundingVolume/AABB.h"
#include "Core/Ray.h"

namespace ph
{

AABB::AABB() : 
	m_minVertex(0, 0, 0), m_maxVertex(0, 0, 0)
{

}

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

// Returned boolean value indicates whether the ray is intersecting the AABB
// or not. If there's an intersection, the near and far hit distance will be
// returned via (out_rayNearHitDist, out_rayFarHitDist); if the ray origin is 
// inside the AABB, near hit distance will be negative.
bool AABB::isIntersecting(const Ray& ray, float32* const out_rayNearHitDist, float32* const out_rayFarHitDist) const
{
	float32 tMin, tMax;

	float32 txMin = (m_minVertex.x - ray.getOrigin().x) / ray.getDirection().x;
	float32 txMax = (m_maxVertex.x - ray.getOrigin().x) / ray.getDirection().x;

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

	float32 tyMin = (m_minVertex.y - ray.getOrigin().y) / ray.getDirection().y;
	float32 tyMax = (m_maxVertex.y - ray.getOrigin().y) / ray.getDirection().y;

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

	float32 tzMin = (m_minVertex.z - ray.getOrigin().z) / ray.getDirection().z;
	float32 tzMax = (m_maxVertex.z - ray.getOrigin().z) / ray.getDirection().z;

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

	if(tMax < 0.0f || tMax < tMin)
		return false;

	*out_rayNearHitDist = tMin;
	*out_rayFarHitDist  = tMax;

	return true;
}

bool AABB::isIntersecting(const AABB& aabb) const
{
	return m_minVertex.x <= aabb.m_maxVertex.x && m_maxVertex.x >= aabb.m_minVertex.x &&
	       m_minVertex.y <= aabb.m_maxVertex.y && m_maxVertex.y >= aabb.m_minVertex.y &&
	       m_minVertex.z <= aabb.m_maxVertex.z && m_maxVertex.z >= aabb.m_minVertex.z;
}

void AABB::unionWith(const AABB& other)
{
	m_minVertex.minLocal(other.getMinVertex());
	m_maxVertex.maxLocal(other.getMaxVertex());
}

}// end namespace ph