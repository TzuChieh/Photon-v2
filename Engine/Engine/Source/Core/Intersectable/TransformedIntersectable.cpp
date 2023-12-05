#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Math/Geometry/TAABB3D.h"

#include <Common/assertion.h>

namespace ph
{

TransformedIntersectable::TransformedIntersectable(
	const Intersectable* const   intersectable,
	const math::Transform* const localToWorld,
	const math::Transform* const worldToLocal) :

	m_intersectable(intersectable),
	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{
	PH_ASSERT(intersectable);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

TransformedIntersectable::TransformedIntersectable(const TransformedIntersectable& other) : 
	m_intersectable(other.m_intersectable),
	m_localToWorld(other.m_localToWorld),
	m_worldToLocal(other.m_worldToLocal)
{}

// FIXME: intersecting routines' time correctness

bool TransformedIntersectable::isOccluding(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_intersectable->isOccluding(localRay);
}

bool TransformedIntersectable::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	if(m_intersectable->isIntersecting(localRay, probe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

void TransformedIntersectable::calcIntersectionDetail(const Ray& ray, HitProbe& probe,
                                                      HitDetail* const out_detail) const
{
	probe.popIntermediateHit();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	HitDetail localDetail;
	probe.getCurrentHit()->calcIntersectionDetail(localRay, probe, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(localDetail.getHitInfo(ECoordSys::WORLD), 
	                          &(out_detail->getHitInfo(ECoordSys::WORLD)));
}

// FIXME: this is broken under timed environment
bool TransformedIntersectable::mayOverlapVolume(const math::AABB3D& aabb) const
{
	math::AABB3D localAABB;
	m_worldToLocal->transform(aabb, &localAABB);
	return m_intersectable->mayOverlapVolume(localAABB);
}

// FIXME: static intersectable do not need to consider time
math::AABB3D TransformedIntersectable::calcAABB() const
{
	const math::AABB3D localAABB = m_intersectable->calcAABB();
		
	math::AABB3D worldAABB;
	m_localToWorld->transform(localAABB, &worldAABB);

	// TODO: modify time interval base on transform properties or aabb size

	// TODO: motions

	//for(size_t i = 0; i < 101; i++)
	//{
	//	Time time;
	//	time.absoluteS = 0;// HACK
	//	time.relativeS = 0;// HACK
	//	time.relativeT = static_cast<real>(1.0 / 100.0 * i);

	//	AABB3D aabb;
	//	m_localToWorld->transform(localAABB, time, &aabb);
	//	worldAABB.unionWith(aabb);
	//}

	return worldAABB;
}

TransformedIntersectable& TransformedIntersectable::operator = (const TransformedIntersectable& rhs)
{
	m_intersectable = rhs.m_intersectable;
	m_localToWorld  = rhs.m_localToWorld;
	m_worldToLocal  = rhs.m_worldToLocal;

	return *this;
}

}// end namespace ph
