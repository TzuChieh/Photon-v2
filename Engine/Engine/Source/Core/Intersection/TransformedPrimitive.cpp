#include "Core/Intersection/TransformedPrimitive.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/HitDetail.h"

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive* const            primitive,
	const math::RigidTransform* const localToWorld,
	const math::RigidTransform* const worldToLocal) :

	Primitive(),

	m_primitive(primitive),
	m_intersectable(primitive, localToWorld, worldToLocal),
	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{
	PH_ASSERT(primitive);
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

real TransformedPrimitive::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	math::Vector3R localPosition;
	m_worldToLocal->transformP(position, &localPosition);
	return m_primitive->calcPositionSamplePdfA(localPosition);
}

void TransformedPrimitive::genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const
{
	m_primitive->genPositionSample(query, sampleFlow);

	if(query.out)
	{
		math::Vector3R worldPosition;
		m_localToWorld->transformP(query.out.position, &worldPosition);

		math::Vector3R worldNormal;
		m_localToWorld->transformO(query.out.normal, &worldNormal);

		query.out.position = worldPosition;
		query.out.normal = worldNormal;
	}
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
