#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/Bound/AABB3D.h"
#include "Common/assertion.h"

namespace ph
{

TransformedIntersectable::TransformedIntersectable(const Intersectable* intersectable,
                                                   const Transform*     localToWorld, 
                                                   const Transform*     worldToLocal) :
	m_intersectable(intersectable),
	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{
	PH_ASSERT(intersectable != nullptr);
	PH_ASSERT(localToWorld  != nullptr);
	PH_ASSERT(worldToLocal  != nullptr);
}

TransformedIntersectable::TransformedIntersectable(const TransformedIntersectable& other) : 
	m_intersectable(other.m_intersectable),
	m_localToWorld(other.m_localToWorld),
	m_worldToLocal(other.m_worldToLocal)
{}

TransformedIntersectable::~TransformedIntersectable() = default;

// FIXME: intersecting routines' time correctness

bool TransformedIntersectable::isIntersecting(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_intersectable->isIntersecting(localRay);
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