#include "Core/BoundingVolume/AABB.h"
#include "Core/Ray.h"

#include <algorithm>

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

// TODO: this method is basically duplicated
bool AABB::isIntersectingVolume(const Ray& ray) const
{
	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	// Note that the following implementation is NaN-aware 
	// (tMin & tMax will never have NaNs)
	real tMin = ray.getMinT();
	real tMax = ray.getMaxT();

	// find ray-slab hitting interval in x-axis then intersect with (tMin, tMax)

	real reciDir = 1.0_r / ray.getDirection().x;
	real t1 = (m_minVertex.x - ray.getOrigin().x) * reciDir;
	real t2 = (m_maxVertex.x - ray.getOrigin().x) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in y-axis then intersect with (tMin, tMax)

	reciDir = 1.0_r / ray.getDirection().y;
	t1 = (m_minVertex.y - ray.getOrigin().y) * reciDir;
	t2 = (m_maxVertex.y - ray.getOrigin().y) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in z-axis then intersect with (tMin, tMax)

	reciDir = 1.0_r / ray.getDirection().z;
	t1 = (m_minVertex.z - ray.getOrigin().z) * reciDir;
	t2 = (m_maxVertex.z - ray.getOrigin().z) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	return true;
}

// Returned boolean value indicates whether the ray is intersecting with the AABB's 
// volume or not. If there's an intersection, the near and far hit distance will be
// returned via (out_rayNearHitDist, out_rayFarHitDist); if the ray origin is inside 
// the AABB, near hit distance will be 0 since volume intersection starts at ray origin.
//
// Reference: Kay and Kayjia's "slab method" from a project of the ACM SIGGRAPH Education 
// Committee named HyperGraph.
bool AABB::isIntersectingVolume(const Ray& ray, 
                                real* const out_rayNearHitDist, real* const out_rayFarHitDist) const
{
	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	// Note that the following implementation is NaN-aware 
	// (tMin & tMax will never have NaNs)
	real tMin = ray.getMinT();
	real tMax = ray.getMaxT();

	// find ray-slab hitting interval in x-axis then intersect with (tMin, tMax)

	real reciDir = 1.0_r / ray.getDirection().x;
	real t1 = (m_minVertex.x - ray.getOrigin().x) * reciDir;
	real t2 = (m_maxVertex.x - ray.getOrigin().x) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in y-axis then intersect with (tMin, tMax)

	reciDir = 1.0_r / ray.getDirection().y;
	t1 = (m_minVertex.y - ray.getOrigin().y) * reciDir;
	t2 = (m_maxVertex.y - ray.getOrigin().y) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	// find ray-slab hitting interval in z-axis then intersect with (tMin, tMax)

	reciDir = 1.0_r / ray.getDirection().z;
	t1 = (m_minVertex.z - ray.getOrigin().z) * reciDir;
	t2 = (m_maxVertex.z - ray.getOrigin().z) * reciDir;

	if(t1 < t2)
	{
		tMin = t1 > tMin ? t1 : tMin;
		tMax = t2 < tMax ? t2 : tMax;
	}
	else
	{
		tMin = t2 > tMin ? t2 : tMin;
		tMax = t1 < tMax ? t1 : tMax;
	}

	if(tMin > tMax)
	{
		return false;
	}

	*out_rayNearHitDist = tMin;
	*out_rayFarHitDist  = tMax;

	return true;
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