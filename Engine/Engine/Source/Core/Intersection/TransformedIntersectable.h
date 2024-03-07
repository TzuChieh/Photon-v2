#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Math/Transform/Transform.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"

#include <Common/assertion.h>

namespace ph
{

class TransformedIntersectable : public Intersectable
{
	// FIXME: intersecting routines' time correctness
public:
	TransformedIntersectable();

	TransformedIntersectable(
		const Intersectable*   intersectable, 
		const math::Transform* localToWorld,
		const math::Transform* worldToLocal);

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override
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

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& /* srcRay */,
		HitProbe& srcProbe) const override
	{
		// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
		// the hit stack; 2. the hit event is invalid
		PH_ASSERT(srcProbe.getCurrentHit() == this);
		srcProbe.popHit();

		return TransformedIntersectable::isIntersecting(ray, probe);
	}

	void calcHitDetail(
		const Ray&       ray, 
		HitProbe&        probe,
		HitDetail* const out_detail) const override
	{
		// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
		// the hit stack; 2. the hit event is invalid
		PH_ASSERT(probe.getCurrentHit() == this);
		probe.popHit();

		Ray localRay;
		m_worldToLocal->transform(ray, &localRay);

		// Current hit is not necessary `m_intersectable`. For example, if `m_intersectable` contains
		// multiple instances then it could simply skip over to one of them.
		PH_ASSERT(probe.getCurrentHit());
		HitDetail localDetail;
		probe.getCurrentHit()->calcHitDetail(localRay, probe, &localDetail);

		*out_detail = localDetail;
		m_localToWorld->transform(
			localDetail.getHitInfo(ECoordSys::World), &(out_detail->getHitInfo(ECoordSys::World)));

		const auto [meanError, maxError] = out_detail->getIntersectErrors();
		out_detail->setIntersectErrors(meanError, maxError * 1.25_r);
	}

	bool isOccluding(const Ray& ray) const override
	{
		Ray localRay;
		m_worldToLocal->transform(ray, &localRay);
		return m_intersectable->isOccluding(localRay);
	}

	bool mayOverlapVolume(const math::AABB3D& aabb) const override;
	math::AABB3D calcAABB() const override;

protected:
	const Intersectable*   m_intersectable;
	const math::Transform* m_localToWorld;
	const math::Transform* m_worldToLocal;
};

}// end namespace ph
