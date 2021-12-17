#include "Core/Intersectable/TransformedPrimitive.h"
#include "Common/assertion.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/Intersectable/Query/PrimitivePosSampleQuery.h"
#include "Core/HitDetail.h"

#include <iostream>

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive* const            primitive,
	const math::RigidTransform* const localToWorld,
	const math::RigidTransform* const worldToLocal) :

	Primitive(primitive != nullptr ? primitive->getMetadata() : nullptr),

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

	if(query.outputs)
	{
		math::Vector3R worldPosition;
		m_localToWorld->transformP(query.outputs.position, &worldPosition);

		math::Vector3R worldNormal;
		m_localToWorld->transformO(query.outputs.normal, &worldNormal);

		query.outputs.position = worldPosition;
		query.outputs.normal = worldNormal;
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
