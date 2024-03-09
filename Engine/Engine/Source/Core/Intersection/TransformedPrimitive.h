#pragma once

#include "Core/Intersection/Primitive.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"

#include <Common/assertion.h>

namespace ph::math { class RigidTransform; }

namespace ph
{

/*!
A transformed primitive accepts only rigid transformations. This way,
properties such as surface area and volume are guaranteed to be the same
during transformations.
*/
class TransformedPrimitive : public Primitive
{
	// FIXME: intersecting routines' time correctness
public:
	TransformedPrimitive(
		const Primitive*            primitive,
		const math::RigidTransform* localToWorld,
		const math::RigidTransform* worldToLocal);

	bool isOccluding(const Ray& ray) const override
	{
		Ray localRay;
		m_worldToLocal->transform(ray, &localRay);
		return m_primitive->isOccluding(localRay);
	}

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override
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

		return TransformedPrimitive::isIntersecting(ray, probe);
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

		// Current hit is not necessary `m_primitive`. For example, if `m_primitive` contains
		// multiple instances then it could simply skip over to one of them.
		PH_ASSERT(probe.getCurrentHit());
		HitDetail localDetail;
		probe.getCurrentHit()->calcHitDetail(localRay, probe, &localDetail);

		*out_detail = localDetail;
		m_localToWorld->transform(
			localDetail.getHitInfo(ECoordSys::World), &(out_detail->getHitInfo(ECoordSys::World)));

		const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
		out_detail->setDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

		// This is a representative of the original primitive
		out_detail->setHitIntrinsics(
			this, 
			out_detail->getUVW(), 
			out_detail->getRayT(),
			out_detail->getFaceID(),
			out_detail->getFaceTopology());
	}

	bool mayOverlapVolume(const math::AABB3D& aabb) const override;
	math::AABB3D calcAABB() const override;

	void genPosSample(
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcPosSamplePdfA(
		PrimitivePosSamplePdfQuery& query,
		HitProbe& probe) const override;

	bool uvwToPosition(
		const math::Vector3R& uvw,
		math::Vector3R*       out_position) const override;

	real calcExtendedArea() const override
	{
		// Does not change under rigid transform
		return m_primitive->calcExtendedArea();
	}

	const PrimitiveMetadata* getMetadata() const override
	{
		return m_primitive->getMetadata();
	}

private:
	const Primitive*            m_primitive;
	const math::RigidTransform* m_localToWorld;
	const math::RigidTransform* m_worldToLocal;
};

}// end namespace ph
