#include "Core/Intersection/TransformedPrimitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Core/HitDetail.h"

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive* const            primitive,
	const math::RigidTransform* const localToWorld,
	const math::RigidTransform* const worldToLocal)

	: Primitive()

	, m_primitive(primitive)
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(primitive);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
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
