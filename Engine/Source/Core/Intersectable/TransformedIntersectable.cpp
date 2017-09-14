#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Core/Bound/AABB3D.h"

namespace ph
{

TransformedIntersectable::TransformedIntersectable(const Intersectable* intersectable,
                                                   const Transform*     localToWorld, 
                                                   const Transform*     worldToLocal) :
	m_intersectable(intersectable),
	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{

}

TransformedIntersectable::TransformedIntersectable(const TransformedIntersectable& other) : 
	m_intersectable(other.m_intersectable),
	m_localToWorld(other.m_localToWorld),
	m_worldToLocal(other.m_worldToLocal)
{

}

TransformedIntersectable::~TransformedIntersectable() = default;

bool TransformedIntersectable::isIntersecting(const Ray& ray,
                                              Intersection* const out_intersection) const
{
	const Time& time = ray.getTime();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	Intersection localIntersection;
	const bool isIntersecting = m_intersectable->isIntersecting(localRay, &localIntersection);
	m_localToWorld->transform(localIntersection, time, out_intersection);

	return isIntersecting;
}

bool TransformedIntersectable::isIntersecting(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	return m_intersectable->isIntersecting(localRay);
}

// FIXME: this is broken under timed environment
bool TransformedIntersectable::isIntersectingVolumeConservative(const AABB3D& aabb) const
{
	AABB3D localAABB;
	m_worldToLocal->transform(aabb, &localAABB);

	return m_intersectable->isIntersectingVolumeConservative(localAABB);
}

void TransformedIntersectable::calcAABB(AABB3D* const out_aabb) const
{
	AABB3D localAABB;
	AABB3D worldAABB;
	m_intersectable->calcAABB(&localAABB);
	m_localToWorld->transform(localAABB, &worldAABB);

	// TODO: modify time interval base on transform properties or aabb size

	for(size_t i = 0; i < 101; i++)
	{
		Time time;
		time.absoluteS = 0;// HACK
		time.relativeS = 0;// HACK
		time.relativeT = static_cast<real>(1.0 / 100.0 * i);

		AABB3D aabb;
		m_localToWorld->transform(localAABB, time, &aabb);
		worldAABB.unionWith(aabb);
	}

	*out_aabb = worldAABB;
}

TransformedIntersectable& TransformedIntersectable::operator = (const TransformedIntersectable& rhs)
{
	m_intersectable = rhs.m_intersectable;
	m_localToWorld  = rhs.m_localToWorld;
	m_worldToLocal  = rhs.m_worldToLocal;

	return *this;
}

}// end namespace ph