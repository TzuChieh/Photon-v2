#pragma once

#include "Core/Intersection/Intersectable.h"
#include "Math/Transform/Transform.h"
#include "Core/Ray.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"

#include <Common/assertion.h>

namespace ph
{

/*! @brief Applies general transformation to an intersectable.
*/
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
		const Ray& srcRay,
		HitProbe& srcProbe) const override
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

	void calcHitDetail(
		const Ray&       ray, 
		HitProbe&        probe,
		HitDetail* const out_detail) const override
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
		out_detail->setDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);
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
