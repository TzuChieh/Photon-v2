#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Intersection/primitive_decorations.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/hash.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <utility>

namespace ph
{

/*! @brief Applies general transformation to an intersectable.
*/
template<typename IntersectableGetter>
class TTransformedIntersectable : public Intersectable
{
	// FIXME: intersecting routines' time correctness
public:
	TTransformedIntersectable(
		IntersectableGetter  intersectableGetter,
		const Transform*     localToWorld,
		const Transform*     worldToLocal);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray&       ray,
		HitProbe&        probe,
		HitDetail* const out_detail) const override;

	bool isOccluding(const Ray& ray) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;

protected:
	[[PH_NO_UNIQUE_ADDRESS]]
	IntersectableGetter m_inner;

	const Transform* m_localToWorld;
	const Transform* m_worldToLocal;
};

template<typename IntersectableGetter>
inline TTransformedIntersectable<IntersectableGetter>::TTransformedIntersectable(
	IntersectableGetter intersectableGetter,
	const Transform* const localToWorld,
	const Transform* const worldToLocal)

	: Intersectable()
	
	, m_inner(std::move(intersectableGetter))
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

template<typename IntersectableGetter>
inline bool TTransformedIntersectable<IntersectableGetter>::isIntersecting(
	const Ray& ray,
	HitProbe& probe) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	if(m_inner().isIntersecting(localRay, probe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IntersectableGetter>
inline bool TTransformedIntersectable<IntersectableGetter>::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& srcRay,
	HitProbe& srcProbe) const
{
	PH_ASSERT(srcProbe.getTopHit() == this);
	srcProbe.popHit();

	Ray localRay, localSrcRay;
	m_worldToLocal->transform(ray, &localRay);
	m_worldToLocal->transform(srcRay, &localSrcRay);
	if(srcProbe.getTopHit()->reintersect(localRay, probe, localSrcRay, srcProbe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename IntersectableGetter>
inline void TTransformedIntersectable<IntersectableGetter>::calcHitDetail(
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

	// Current hit is not necessary `m_inner()`. For example, if `m_inner()` contains
	// multiple instances then it could simply skip over to one of them.
	PH_ASSERT(probe.getTopHit());
	HitDetail localDetail;
	probe.getTopHit()->calcHitDetail(localRay, probe, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(
		localDetail.getHitInfo(ECoordSys::World), &(out_detail->hitInfo(ECoordSys::World)));

	const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
	out_detail->updateDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

	out_detail->updateGlobalPrimitiveID(math::combine_hashes(
		out_detail->getGlobalPrimitiveID(),
		math::moremur_bit_mix_64(reinterpret_cast<uint64>(m_worldToLocal))));
}

template<typename IntersectableGetter>
inline bool TTransformedIntersectable<IntersectableGetter>::isOccluding(
	const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_inner().isOccluding(localRay);
}

template<typename IntersectableGetter>
inline bool TTransformedIntersectable<IntersectableGetter>::mayOverlapVolume(
	const math::AABB3D& volume) const
{
	return calcAABB().isIntersectingVolume(volume);
}

template<typename IntersectableGetter>
inline math::AABB3D TTransformedIntersectable<IntersectableGetter>::calcAABB() const
{
	const Time startTime(0, 0);
	const Time endTime(0, 1);
	const math::AABB3D localAABB = m_inner().calcAABB();

	math::AABB3D worldAABB;
	if(m_localToWorld->hasMotion(startTime, endTime))
	{
		m_localToWorld->calcSweepAABB(localAABB, startTime, endTime, &worldAABB);
	}
	else
	{
		m_localToWorld->transform(localAABB, startTime, &worldAABB);
	}

	return worldAABB;
}

}// end namespace ph
