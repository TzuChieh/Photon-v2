#include "Core/Intersection/TransformedPrimitive.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosSamplePdfQuery.h"
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
		localQuery.inputs.set(query.inputs.getTime(), localObservationPos);
	}
	else
	{
		localQuery.inputs.set(query.inputs.getTime());
	}

	m_primitive->genPosSample(localQuery, sampleFlow, probe);
	if(!localQuery.outputs)
	{
		query.outputs.invalidate();
		return;
	}

	probe.pushIntermediateHit(this);

	math::Vector3R worldPos;
	m_localToWorld->transformP(localQuery.outputs.getPos(), &worldPos);
	query.outputs.setPos(worldPos);

	Ray worldRay;
	m_localToWorld->transform(localQuery.outputs.getObservationRay(), &worldRay);
	query.outputs.setObservationRay(worldRay);

	query.outputs.setPdfA(localQuery.outputs.getPdfA());
}

void TransformedPrimitive::calcPosSamplePdfA(
	PrimitivePosSamplePdfQuery& query,
	HitProbe& probe) const
{
	math::Vector3R localPosition;
	m_worldToLocal->transformP(query.inputs.getPos(), &localPosition);

	Ray localRay;
	m_worldToLocal->transform(query.inputs.getObservationRay(), &localRay);

	PrimitivePosSamplePdfQuery localQuery;
	localQuery.inputs.set(localPosition, localRay, query.inputs.getFaceID());

	m_primitive->calcPosSamplePdfA(localQuery, probe);
	if(!localQuery.outputs)
	{
		query.outputs.setPdfA(0);
		return;
	}

	probe.pushIntermediateHit(this);

	query.outputs.setPdfA(localQuery.outputs.getPdfA());
}

bool TransformedPrimitive::uvwToPosition(
	const math::Vector3R& uvw,
	math::Vector3R* const out_position) const
{
	PH_ASSERT(out_position);

	math::Vector3R localPosition;
	if(!m_primitive->uvwToPosition(uvw, &localPosition))
	{
		return false;
	}

	m_localToWorld->transformP(localPosition, out_position);
	return true;
}

}// end namespace ph
