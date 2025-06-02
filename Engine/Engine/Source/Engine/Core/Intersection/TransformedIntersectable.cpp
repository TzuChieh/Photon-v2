#include "Engine/Core/Intersection/TransformedIntersectable.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/hash.h"

namespace ph
{

TransformedIntersectable::TransformedIntersectable()

	: Intersectable()

	, m_intersectable(nullptr)
	, m_localToWorld (nullptr)
	, m_worldToLocal (nullptr)
{}

TransformedIntersectable::TransformedIntersectable(
	const Intersectable* const   intersectable,
	const math::Transform* const localToWorld,
	const math::Transform* const worldToLocal)

	: m_intersectable(intersectable)
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(intersectable);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

void TransformedIntersectable::calcHitDetail(
	const Ray&       ray, 
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
	// the hit stack; 2. the hit event is invalid
	PH_ASSERT(probe.getTopHit() == this);
	probe.popHit();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	// Current hit is not necessary `m_intersectable`. For example, if `m_intersectable` contains
	// multiple instances then it could simply skip over to one of them.
	PH_ASSERT(probe.getTopHit());
	HitDetail localDetail;
	probe.getTopHit()->calcHitDetail(localRay, probe, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(
		localDetail.getHitInfo(ECoordSys::World), &(out_detail->getHitInfo(ECoordSys::World)));

	const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
	out_detail->updateDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

	out_detail->updateGlobalPrimitiveID(math::combine_hashes(
		out_detail->getGlobalPrimitiveID(),
		math::moremur_bit_mix_64(reinterpret_cast<uint64>(m_worldToLocal))));
}

bool TransformedIntersectable::mayOverlapVolume(const math::AABB3D& aabb) const
{
	// FIXME: this is broken under timed environment

	math::AABB3D localAABB;
	m_worldToLocal->transform(aabb, &localAABB);
	return m_intersectable->mayOverlapVolume(localAABB);
}

math::AABB3D TransformedIntersectable::calcAABB() const
{
	// FIXME: static intersectable do not need to consider time

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

}// end namespace ph
