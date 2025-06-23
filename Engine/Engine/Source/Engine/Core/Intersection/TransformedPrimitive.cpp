#include "Engine/Core/Intersection/TransformedPrimitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Math/hash.h"

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive* const      primitive,
	const RigidTransform* const localToWorld,
	const RigidTransform* const worldToLocal)

	: Primitive()

	, m_primitive(primitive)
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(primitive);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

void TransformedPrimitive::calcHitDetail(
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

	// Current hit is not necessary `m_primitive`. For example, if `m_primitive` contains
	// multiple instances then it could simply skip over to one of them.
	PH_ASSERT(probe.getTopHit());
	HitDetail localDetail;
	probe.getTopHit()->calcHitDetail(localRay, probe, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(
		localDetail.getHitInfo(ECoordSys::World), &(out_detail->hitInfo(ECoordSys::World)));

	const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
	out_detail->updateDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

	// This is a representative of the original primitive
	out_detail->updatePrimitive(this);

	out_detail->updateGlobalPrimitiveID(math::combine_hashes(
		out_detail->getGlobalPrimitiveID(),
		math::moremur_bit_mix_64(reinterpret_cast<uint64>(m_worldToLocal))));
}

bool TransformedPrimitive::mayOverlapVolume(const math::AABB3D& aabb) const
{
	// FIXME: this is broken under timed environment

	math::AABB3D localAABB;
	m_worldToLocal->transform(aabb, &localAABB);
	return m_primitive->mayOverlapVolume(localAABB);
}

math::AABB3D TransformedPrimitive::calcAABB() const
{
	// FIXME: static intersectable do not need to consider time

	const math::AABB3D localAABB = m_primitive->calcAABB();

	math::AABB3D worldAABB;
	m_localToWorld->transform(localAABB, &worldAABB);
	return worldAABB;
}

void TransformedPrimitive::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PrimitivePosSampleQuery localQuery;
	if(query.inputs.getObservationPos())
	{
		math::Vector3R localObservationPos;
		m_worldToLocal->transformP(*query.inputs.getObservationPos(), &localObservationPos);

		localQuery.inputs.set(
			query.inputs.getTime(),
			localObservationPos,
			query.inputs.getUvw(),
			query.inputs.getUvwPdf(),
			query.inputs.suggestDir());
	}
	else
	{
		localQuery.inputs = query.inputs;
	}

	m_primitive->genPosSample(localQuery, sampleFlow, probe);
	if(!localQuery.outputs)
	{
		return;
	}

	probe.pushIntermediateHit(this);

	// Make sure to forward all local query outputs to world query outputs

	math::Vector3R worldPos;
	m_localToWorld->transformP(localQuery.outputs.getPos(), &worldPos);
	query.outputs.setPos(worldPos);

	Ray worldRay;
	m_localToWorld->transform(localQuery.outputs.getObservationRay(), &worldRay);
	query.outputs.setObservationRay(worldRay);

	query.outputs.setPdfPos(localQuery.outputs.getPdfPos());
	query.outputs.setPdfDir(localQuery.outputs.getPdfDir());
}

void TransformedPrimitive::calcPosPdf(PrimitivePosPdfQuery& query) const
{
	m_primitive->calcPosPdf(query);
}

}// end namespace ph
