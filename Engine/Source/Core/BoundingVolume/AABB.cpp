#include "Core/BoundingVolume/AABB.h"
#include "Core/Ray.h"

namespace ph
{

AABB::AABB() : 
	m_minVertex(0, 0, 0), m_maxVertex(0, 0, 0)
{

}

AABB::AABB(const Vector3R& minVertex, const Vector3R& maxVertex) :
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{

}

bool AABB::isIntersectingVolume(const Ray& ray) const
{
	// Reference: Kay and Kayjia's "slab method" from a project of the ACM SIGGRAPH Education Committee
	// named HyperGraph.
	// Note: This algorithm can produce NaNs which will generate false positives in rare cases. Although 
	// it can be handled, we ignore it since the performance will degrade, also bounding volumes are allowed 
	// to produce false positives already.

	real tMin, tMax;

	const real txMin = (m_minVertex.x - ray.getOrigin().x) / ray.getDirection().x;
	const real txMax = (m_maxVertex.x - ray.getOrigin().x) / ray.getDirection().x;
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

	const real tyMin = (m_minVertex.y - ray.getOrigin().y) / ray.getDirection().y;
	const real tyMax = (m_maxVertex.y - ray.getOrigin().y) / ray.getDirection().y;
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

	const real tzMin = (m_minVertex.z - ray.getOrigin().z) / ray.getDirection().z;
	const real tzMax = (m_maxVertex.z - ray.getOrigin().z) / ray.getDirection().z;
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

	// below conditions are NaN-aware

	// ray intersects with AABB if elongated in both directions with unlimited length
	if(tMax > tMin)
	{
		// for a length-limited ray to actually intersect with AABB, it must overlap (tMin, tMax)
		if(ray.getMaxT() > tMin && ray.getMinT() < tMax)
		{
			return true;
		}
	}

	return false;
}

// Returned boolean value indicates whether the ray is intersecting with the AABB's 
// volume or not. If there's an intersection, the near and far hit distance will be
// returned via (out_rayNearHitDist, out_rayFarHitDist); if the ray origin is inside 
// the AABB, near hit distance will be 0 since volume intersection starts at ray origin.
bool AABB::isIntersectingVolume(const Ray& ray, real* const out_rayNearHitDist, real* const out_rayFarHitDist) const
{
	real tMin, tMax;

	real txMin = (m_minVertex.x - ray.getOrigin().x) / ray.getDirection().x;
	real txMax = (m_maxVertex.x - ray.getOrigin().x) / ray.getDirection().x;

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

	real tyMin = (m_minVertex.y - ray.getOrigin().y) / ray.getDirection().y;
	real tyMax = (m_maxVertex.y - ray.getOrigin().y) / ray.getDirection().y;

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

	real tzMin = (m_minVertex.z - ray.getOrigin().z) / ray.getDirection().z;
	real tzMax = (m_maxVertex.z - ray.getOrigin().z) / ray.getDirection().z;

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

	// below conditions are NaN-aware

	// ray intersects with AABB if elongated in both directions with unlimited length
	if(tMax > tMin)
	{
		// for a length-limited ray to actually intersect with AABB, it must overlap (tMin, tMax)
		if(ray.getMaxT() > tMin && ray.getMinT() < tMax)
		{
			*out_rayNearHitDist = tMin < ray.getMinT() ? ray.getMinT() : tMin;
			*out_rayFarHitDist  = tMax > ray.getMaxT() ? ray.getMaxT() : tMax;
			return true;
		}
	}

	return false;
}

bool AABB::isIntersectingVolume(const AABB& aabb) const
{
	return m_minVertex.x < aabb.m_maxVertex.x && m_maxVertex.x > aabb.m_minVertex.x &&
	       m_minVertex.y < aabb.m_maxVertex.y && m_maxVertex.y > aabb.m_minVertex.y &&
	       m_minVertex.z < aabb.m_maxVertex.z && m_maxVertex.z > aabb.m_minVertex.z;
}

void AABB::unionWith(const AABB& other)
{
	m_minVertex.minLocal(other.getMinVertex());
	m_maxVertex.maxLocal(other.getMaxVertex());
}

}// end namespace ph