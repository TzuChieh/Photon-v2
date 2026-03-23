#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Transform/RigidTransform.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/Ray.h"

#include <Common/assertion.h>

namespace ph::math { class RigidTransform; }

namespace ph
{

/*! @brief Applies rigid transformation to a primitive.
A transformed primitive accepts only rigid transformations. This way,
properties such as surface area and volume are guaranteed to be the same
during transformations.
*/
class TransformedPrimitive : public Primitive
{
	// FIXME: intersecting routines' time correctness
public:
	TransformedPrimitive(
		const Primitive*      primitive,
		const RigidTransform* localToWorld,
		const RigidTransform* worldToLocal);

	bool isOccluding(const Ray& ray) const override;

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

	bool mayOverlapVolume(const math::AABB3D& aabb) const override;
	math::AABB3D calcAABB() const override;

	void genPosSample(
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcPosPdf(PrimitivePosPdfQuery& query) const override;

	real calcExtendedArea() const override;

	const PrimitiveMetadata& getMetadata(uint32 slot) const override;

private:
	const Primitive*      m_primitive;
	const RigidTransform* m_localToWorld;
	const RigidTransform* m_worldToLocal;
};

inline bool TransformedPrimitive::isOccluding(const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_primitive->isOccluding(localRay);
}

inline bool TransformedPrimitive::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	if(m_primitive->isIntersecting(localRay, probe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

inline bool TransformedPrimitive::reintersect(
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

inline real TransformedPrimitive::calcExtendedArea() const
{
	// Does not change under rigid transform
	return m_primitive->calcExtendedArea();
}

inline const PrimitiveMetadata& TransformedPrimitive::getMetadata(const uint32 slot) const
{
	return m_primitive->getMetadata(slot);
}

}// end namespace ph
