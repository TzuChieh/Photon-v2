#include "Core/Intersectable/TransformedPrimitive.h"
#include "Common/assertion.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/Sample/PositionSample.h"
#include "Core/HitDetail.h"

#include <iostream>

namespace ph
{

TransformedPrimitive::TransformedPrimitive(
	const Primitive*      const primitive,
	const RigidTransform* const localToWorld,
	const RigidTransform* const worldToLocal) : 

	Primitive(primitive != nullptr ? primitive->getMetadata() : nullptr),

	m_primitive(primitive),
	m_intersectable(primitive, localToWorld, worldToLocal),
	m_localToWorld(localToWorld),
	m_worldToLocal(worldToLocal)
{
	PH_ASSERT(primitive    != nullptr);
	PH_ASSERT(localToWorld != nullptr);
	PH_ASSERT(worldToLocal != nullptr);
}

TransformedPrimitive::~TransformedPrimitive() = default;

real TransformedPrimitive::calcPositionSamplePdfA(const Vector3R& position) const
{
	Vector3R localPosition;
	m_worldToLocal->transformP(position, &localPosition);
	return m_primitive->calcPositionSamplePdfA(localPosition);
}

void TransformedPrimitive::genPositionSample(PositionSample* const out_sample) const
{
	PH_ASSERT(out_sample);

	PositionSample localSample;
	m_primitive->genPositionSample(&localSample);

	m_localToWorld->transformP(localSample.position, &out_sample->position);
	m_localToWorld->transformO(localSample.normal,   &out_sample->normal);
	out_sample->uvw = localSample.uvw;
	out_sample->pdf = localSample.pdf;
}

bool TransformedPrimitive::uvwToPosition(
	const Vector3R& uvw,
	const Vector3R& observationPoint,
	Vector3R* const out_position) const
{
	PH_ASSERT(out_position);

	Vector3R localObservationPoint;
	m_localToWorld->transformP(observationPoint, &localObservationPoint);

	Vector3R localPosition;
	if(!m_primitive->uvwToPosition(uvw, localObservationPoint, &localPosition))
	{
		return false;
	}

	m_localToWorld->transformP(localPosition, out_position);
	return true;
}

}// end namespace ph