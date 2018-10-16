#include "Core/Bound/AABB3D.h"
#include "Core/Ray.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>

namespace ph
{

AABB3D AABB3D::makeUnioned(const AABB3D& a, const AABB3D& b)
{
	return AABB3D(a).unionWith(b);
}

AABB3D::AABB3D() :
	AABB3D(Vector3R(0, 0, 0))
{}

AABB3D::AABB3D(const Vector3R& point) :
	AABB3D(point, point)
{}

AABB3D::AABB3D(const Vector3R& minVertex, const Vector3R& maxVertex) :
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{
	PH_ASSERT(maxVertex.x >= minVertex.x &&
	          maxVertex.y >= minVertex.y &&
	          maxVertex.z >= minVertex.z);
}

// REFACTOR: this method is duplicated with isIntersectingVolume(3)
bool AABB3D::isIntersectingVolume(const Ray& ray) const
{
	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	//
	// Note that the following implementation is NaN-aware 
	// (tMin & tMax will never have NaNs)

	PH_ASSERT(!std::isnan(ray.getMinT()) && !std::isnan(ray.getMaxT()));

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

/*
	Returned boolean value indicates whether the ray is intersecting with the
	AABB's volume or not. If there's an intersection, the near and far hit
	distances will be returned via (out_rayNearHitDist, out_rayFarHitDist); if
	the starting point of the ray is inside the AABB, near hit distance will be
	the ray's t_min since volume intersection starts immediately on the ray's 
	starting point.

	Reference: Kay and Kayjia's "slab method" from a project of the ACM 
	SIGGRAPH Education Committee named HyperGraph.
*/
bool AABB3D::isIntersectingVolume(
	const Ray&  ray,
	real* const out_rayNearHitT, 
	real* const out_rayFarHitT) const
{
	PH_ASSERT(out_rayNearHitT && out_rayFarHitT);

	// The starting ray interval (tMin, tMax) will be incrementally intersect
	// against each ray-slab hitting interval (t1, t2) and be updated with the
	// resulting interval.
	//
	// Note that the following implementation is NaN-aware 
	// (tMin & tMax will never have NaNs)

	PH_ASSERT(!std::isnan(ray.getMinT()) && !std::isnan(ray.getMaxT()));

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

	*out_rayNearHitT = tMin;
	*out_rayFarHitT  = tMax;

	return true;
}

bool AABB3D::isIntersectingVolume(const AABB3D& other) const
{
	return m_minVertex.x <= other.m_maxVertex.x && m_maxVertex.x >= other.m_minVertex.x &&
	       m_minVertex.y <= other.m_maxVertex.y && m_maxVertex.y >= other.m_minVertex.y &&
	       m_minVertex.z <= other.m_maxVertex.z && m_maxVertex.z >= other.m_minVertex.z;
}

AABB3D& AABB3D::unionWith(const AABB3D& other)
{
	m_minVertex.minLocal(other.getMinVertex());
	m_maxVertex.maxLocal(other.getMaxVertex());

	return *this;
}

AABB3D& AABB3D::unionWith(const Vector3R& point)
{
	m_minVertex.minLocal(point);
	m_maxVertex.maxLocal(point);

	return *this;
}

std::vector<Vector3R> AABB3D::getVertices() const
{
	return {Vector3R(m_minVertex.x, m_minVertex.y, m_minVertex.z),
	        Vector3R(m_maxVertex.x, m_minVertex.y, m_minVertex.z),
	        Vector3R(m_minVertex.x, m_maxVertex.y, m_minVertex.z),
	        Vector3R(m_minVertex.x, m_minVertex.y, m_maxVertex.z),
	        Vector3R(m_maxVertex.x, m_maxVertex.y, m_minVertex.z),
	        Vector3R(m_minVertex.x, m_maxVertex.y, m_maxVertex.z),
	        Vector3R(m_maxVertex.x, m_minVertex.y, m_maxVertex.z),
	        Vector3R(m_maxVertex.x, m_maxVertex.y, m_maxVertex.z)};
}

std::string AABB3D::toString() const
{
	return "min-vertex = " + m_minVertex.toString() + ", max-vertex = " + m_maxVertex.toString();
}

}// end namespace ph