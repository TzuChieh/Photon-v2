#include "Core/Intersectable/TransformedPrimitive.h"
#include "Common/assertion.h"
#include "Math/Transform/RigidTransform.h"
#include "Core/Sample/PositionSample.h"

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
	PH_ASSERT(out_sample != nullptr);

	PositionSample localSample;
	m_primitive->genPositionSample(&localSample);

	m_localToWorld->transformP(localSample.position, &out_sample->position);
	m_localToWorld->transformO(localSample.normal,   &out_sample->normal);
	out_sample->uvw = localSample.uvw;
	out_sample->pdf = localSample.pdf;
}

}// end namespace ph