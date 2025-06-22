#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/HitProbe.h"

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
		const Intersectable* intersectable, 
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

	bool mayOverlapVolume(const math::AABB3D& aabb) const override;
	math::AABB3D calcAABB() const override;

protected:
	const Intersectable* m_intersectable;
	const Transform*     m_localToWorld;
	const Transform*     m_worldToLocal;
};

inline bool TransformedIntersectable::isIntersecting(const Ray& ray, HitProbe& probe) const
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

inline bool TransformedIntersectable::reintersect(
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

inline bool TransformedIntersectable::isOccluding(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_intersectable->isOccluding(localRay);
}

}// end namespace ph
