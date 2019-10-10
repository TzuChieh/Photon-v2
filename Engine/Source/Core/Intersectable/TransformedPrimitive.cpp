#include "Core/Intersectable/TransformedPrimitive.h"
#include "Common/assertion.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/Sample/PositionSample.h"
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

void TransformedPrimitive::genPositionSample(PositionSample* const out_sample) const
{
	PH_ASSERT(out_sample);

	PositionSample localSample;
	m_primitive->genPositionSample(&localSample);

	if(localSample.pdf > 0)
	{
		m_localToWorld->transformP(localSample.position, &out_sample->position);
		m_localToWorld->transformO(localSample.normal, &out_sample->normal);
		out_sample->uvw = localSample.uvw;
		out_sample->pdf = localSample.pdf;
	}
	else
	{
		out_sample->pdf = 0;
	}
}

bool TransformedPrimitive::uvwToPosition(
	const math::Vector3R& uvw,
	const math::Vector3R& observationPoint,
	math::Vector3R* const out_position) const
{
	PH_ASSERT(out_position);

	math::Vector3R localObservationPoint;
	m_localToWorld->transformP(observationPoint, &localObservationPoint);

	math::Vector3R localPosition;
	if(!m_primitive->uvwToPosition(uvw, localObservationPoint, &localPosition))
	{
		return false;
	}

	m_localToWorld->transformP(localPosition, out_position);
	return true;
}

}// end namespace ph
