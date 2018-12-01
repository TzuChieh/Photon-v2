#pragma once

#include "Core/Bound/TAABB3D.h"
#include "Core/Ray.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>

namespace ph
{

template<typename T>
inline TAABB3D<T> TAABB3D<T>::makeUnioned(const TAABB3D& a, const TAABB3D& b)
{
	return TAABB3D(a).unionWith(b);
}

template<typename T>
inline TAABB3D<T>::TAABB3D() :
	TAABB3D(TVector3<T>(0, 0, 0))
{}

template<typename T>
inline TAABB3D<T>::TAABB3D(const TVector3<T>& point) :
	TAABB3D(point, point)
{}

template<typename T>
inline TAABB3D<T>::TAABB3D(const TVector3<T>& minVertex, const TVector3<T>& maxVertex) :
	m_minVertex(minVertex), m_maxVertex(maxVertex)
{
	PH_ASSERT_MSG(maxVertex.x >= minVertex.x &&
	              maxVertex.y >= minVertex.y &&
	              maxVertex.z >= minVertex.z, 
		"minVertex = " + minVertex.toString() +
		"maxVertex = " + maxVertex.toString());
}

// REFACTOR: this method is duplicated with isIntersectingVolume(3)
template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(const Ray& ray) const
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

	real reciDir = 1.0_r / static_cast<real>(ray.getDirection().x);
	real t1 = (m_minVertex.x - static_cast<real>(ray.getOrigin().x)) * reciDir;
	real t2 = (m_maxVertex.x - static_cast<real>(ray.getOrigin().x)) * reciDir;

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

	reciDir = 1.0_r / static_cast<real>(ray.getDirection().y);
	t1 = (m_minVertex.y - static_cast<real>(ray.getOrigin().y)) * reciDir;
	t2 = (m_maxVertex.y - static_cast<real>(ray.getOrigin().y)) * reciDir;

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

	reciDir = 1.0_r / static_cast<real>(ray.getDirection().z);
	t1 = (m_minVertex.z - static_cast<real>(ray.getOrigin().z)) * reciDir;
	t2 = (m_maxVertex.z - static_cast<real>(ray.getOrigin().z)) * reciDir;

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
template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(
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

	real reciDir = 1.0_r / static_cast<real>(ray.getDirection().x);
	real t1 = (m_minVertex.x - static_cast<real>(ray.getOrigin().x)) * reciDir;
	real t2 = (m_maxVertex.x - static_cast<real>(ray.getOrigin().x)) * reciDir;

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

	reciDir = 1.0_r / static_cast<real>(ray.getDirection().y);
	t1 = (m_minVertex.y - static_cast<real>(ray.getOrigin().y)) * reciDir;
	t2 = (m_maxVertex.y - static_cast<real>(ray.getOrigin().y)) * reciDir;

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

	reciDir = 1.0_r / static_cast<real>(ray.getDirection().z);
	t1 = (m_minVertex.z - static_cast<real>(ray.getOrigin().z)) * reciDir;
	t2 = (m_maxVertex.z - static_cast<real>(ray.getOrigin().z)) * reciDir;

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

template<typename T>
inline bool TAABB3D<T>::isIntersectingVolume(const TAABB3D& other) const
{
	return m_minVertex.x <= other.m_maxVertex.x && m_maxVertex.x >= other.m_minVertex.x &&
	       m_minVertex.y <= other.m_maxVertex.y && m_maxVertex.y >= other.m_minVertex.y &&
	       m_minVertex.z <= other.m_maxVertex.z && m_maxVertex.z >= other.m_minVertex.z;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::unionWith(const TAABB3D& other)
{
	m_minVertex.minLocal(other.getMinVertex());
	m_maxVertex.maxLocal(other.getMaxVertex());

	return *this;
}

template<typename T>
inline TAABB3D<T>& TAABB3D<T>::unionWith(const TVector3<T>& point)
{
	m_minVertex.minLocal(point);
	m_maxVertex.maxLocal(point);

	return *this;
}

template<typename T>
inline std::vector<TVector3<T>> TAABB3D<T>::getVertices() const
{
	return {TVector3<T>(m_minVertex.x, m_minVertex.y, m_minVertex.z),
	        TVector3<T>(m_maxVertex.x, m_minVertex.y, m_minVertex.z),
	        TVector3<T>(m_minVertex.x, m_maxVertex.y, m_minVertex.z),
	        TVector3<T>(m_minVertex.x, m_minVertex.y, m_maxVertex.z),
	        TVector3<T>(m_maxVertex.x, m_maxVertex.y, m_minVertex.z),
	        TVector3<T>(m_minVertex.x, m_maxVertex.y, m_maxVertex.z),
	        TVector3<T>(m_maxVertex.x, m_minVertex.y, m_maxVertex.z),
	        TVector3<T>(m_maxVertex.x, m_maxVertex.y, m_maxVertex.z)};
}

template<typename T>
inline void TAABB3D<T>::getMinMaxVertices(TVector3<T>* const out_minVertex, TVector3<T>* const out_maxVertex) const
{
	PH_ASSERT(out_minVertex && out_maxVertex);

	out_minVertex->set(m_minVertex);
	out_maxVertex->set(m_maxVertex);
}

template<typename T>
inline const TVector3<T>& TAABB3D<T>::getMinVertex() const
{
	return m_minVertex;
}

template<typename T>
inline const TVector3<T>& TAABB3D<T>::getMaxVertex() const
{
	return m_maxVertex;
}

template<typename T>
inline Vector3R TAABB3D<T>::getCentroid() const
{
	return Vector3R(m_minVertex.add(m_maxVertex)).mulLocal(0.5_r);
}

template<typename T>
inline TVector3<T> TAABB3D<T>::getExtents() const
{
	return m_maxVertex.sub(m_minVertex);
}

template<typename T>
inline T TAABB3D<T>::getSurfaceArea() const
{
	const TVector3<T>& extents = getExtents();
	return T(2) * (extents.x * extents.y + extents.y * extents.z + extents.z * extents.x);
}

template<typename T>
inline T TAABB3D<T>::getVolume() const
{
	const TVector3<T>& extents = getExtents();
	return extents.x * extents.y * extents.z;
}

template<typename T>
inline void TAABB3D<T>::setMinVertex(const TVector3<T>& minVertex)
{
	m_minVertex = minVertex;
}

template<typename T>
inline void TAABB3D<T>::setMaxVertex(const TVector3<T>& maxVertex)
{
	m_maxVertex = maxVertex;
}

template<typename T>
inline void TAABB3D<T>::expand(const TVector3<T>& amount)
{
	m_minVertex.subLocal(amount);
	m_maxVertex.addLocal(amount);
}

template<typename T>
inline bool TAABB3D<T>::isPoint() const
{
	return m_minVertex.equals(m_maxVertex);
}

template<typename T>
inline bool TAABB3D<T>::isFiniteVolume() const
{
	return std::isnormal(getVolume());
}

template<typename T>
inline std::pair<TAABB3D<T>, TAABB3D<T>> TAABB3D<T>::getSplitted(const int axis, const T splitPoint) const
{
	PH_ASSERT_GE(getExtents()[axis], splitPoint);
	PH_ASSERT_LE(splitPoint, getExtents()[axis]);

	return {TAABB3D(m_minVertex, TVector3<T>(m_maxVertex).set(axis, splitPoint)),
	        TAABB3D(TVector3<T>(m_minVertex).set(axis, splitPoint), m_maxVertex)};
}

template<typename T>
std::string TAABB3D<T>::toString() const
{
	return "min-vertex = " + m_minVertex.toString() + ", max-vertex = " + m_maxVertex.toString();
}

}// end namespace ph