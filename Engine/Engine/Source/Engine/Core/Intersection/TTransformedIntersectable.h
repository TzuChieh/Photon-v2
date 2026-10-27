#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Intersection/primitive_decorations.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Math/Geometry/TAABB3D.h"
#include "Engine/Math/hash.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <utility>

namespace ph
{

/*! @brief Applies general transformation to an intersectable.
@tparam SHOULD_FLIP_NG Whether to flip the world-space geometric normal.
*/
template<typename IntersectableGetter, bool SHOULD_FLIP_NG = false>
class TTransformedIntersectable : public Intersectable
{
	// FIXME: intersecting routines' time correctness
public:
	TTransformedIntersectable(
		IntersectableGetter  intersectableGetter,
		const Transform*     localToWorld,
		const Transform*     worldToLocal);

	bool isIntersecting(HitEvent& event) const override;

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override;

	void calcHitDetail(HitEvent& event, HitDetail* const out_detail) const override;

	bool isOccluding(const Ray& ray) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;

protected:
	[[PH_NO_UNIQUE_ADDRESS]]
	IntersectableGetter m_inner;

	const Transform* m_localToWorld;
	const Transform* m_worldToLocal;
};

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::TTransformedIntersectable(
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

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::isIntersecting(
	HitEvent& event) const
{
	Ray localRay;
	m_worldToLocal->transform(event.getRay(), &localRay);

	const bool hasHit = event.withRay(
		localRay,
		[this](HitEvent& localEvent)
		{
			return m_inner().isIntersecting(localEvent);
		});

	if(hasHit)
	{
		event.pushIntermediateHit(this);
	}

	return hasHit;
}

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::reintersect(
	HitEvent& event,
	HitEvent& srcEvent) const
{
	PH_ASSERT(srcEvent.getTopHit() == this);
	srcEvent.popHit();

	Ray localRay, localSrcRay;
	m_worldToLocal->transform(event.getRay(), &localRay);
	m_worldToLocal->transform(srcEvent.getRay(), &localSrcRay);

	srcEvent.setRay(localSrcRay);
	const bool hasHit = event.withRay(
		localRay,
		[&srcEvent](HitEvent& localEvent)
		{
			return srcEvent.getTopHit()->reintersect(localEvent, srcEvent);
		});
		
	if(hasHit)
	{
		event.pushIntermediateHit(this);
	}
	
	return hasHit;
}

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline void TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::calcHitDetail(
	HitEvent& event,
	HitDetail* const out_detail) const
{
	// If failed, it is likely to be caused by: 1. mismatched/missing event push or pop in
	// the hit stack; 2. the hit event is invalid
	PH_ASSERT(event.getTopHit() == this);
	event.popHit();

	Ray localRay;
	m_worldToLocal->transform(event.getRay(), &localRay);

	// Current hit is not necessary `m_inner()`. For example, if `m_inner()` contains
	// multiple instances then it could simply skip over to one of them.
	PH_ASSERT(event.getTopHit());
	HitDetail localDetail;
	event.setRay(localRay);
	event.getTopHit()->calcHitDetail(event, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(
		localDetail.getHitInfo(ECoordSys::World), &(out_detail->hitInfo(ECoordSys::World)));

	const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
	out_detail->updateDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

	out_detail->updateGlobalPrimitiveID(math::combine_hashes(
		out_detail->getGlobalPrimitiveID(),
		math::moremur_bit_mix_64(reinterpret_cast<uint64>(m_worldToLocal))));

	if constexpr(SHOULD_FLIP_NG)
	{
		out_detail->setFlippedGeometryNormal();
	}
}

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::isOccluding(
	const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_inner().isOccluding(localRay);
}

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::mayOverlapVolume(
	const math::AABB3D& volume) const
{
	return calcAABB().isIntersectingVolume(volume);
}

template<typename IntersectableGetter, bool SHOULD_FLIP_NG>
inline math::AABB3D TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>::calcAABB() const
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
