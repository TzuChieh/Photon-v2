#include "Core/Intersectable/SuperpositionedPrimitive.h"
#include "Core/HitProbe.h"

#include <iostream>

namespace ph
{

SuperpositionedPrimitive::SuperpositionedPrimitive(
	const PrimitiveMetadata* metadata,
	const Primitive*         mainPrimitive) : 

	SuperpositionedPrimitive(metadata, {mainPrimitive}, 0)
{}

SuperpositionedPrimitive::SuperpositionedPrimitive(
	const PrimitiveMetadata*             metadata,
	const std::vector<const Primitive*>& primitives,
	const std::size_t                    mainPrimitiveIndex) : 

	Primitive(metadata),

	m_primitives(primitives),
	m_mainPrimitive(nullptr)
{
	PH_ASSERT(mainPrimitiveIndex < primitives.size());

	m_mainPrimitive = primitives[mainPrimitiveIndex];
}

bool SuperpositionedPrimitive::isOccluding(const Ray& ray) const
{
	PH_ASSERT(m_mainPrimitive);

	return m_mainPrimitive->isOccluding(ray);
}

bool SuperpositionedPrimitive::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	PH_ASSERT(m_mainPrimitive);

	if(m_mainPrimitive->isIntersecting(ray, probe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

void SuperpositionedPrimitive::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	probe.popIntermediateHit();

	const uint32 channel = probe.getChannel();

	const Primitive* targetPrimitive;
	if(channel < m_primitives.size())
	{
		targetPrimitive = m_primitives[channel];
	}
	else
	{
		targetPrimitive = m_mainPrimitive;
	}

	// substitutes next hit with target primitive

	PH_ASSERT(probe.getCurrentHit() == m_mainPrimitive);
	probe.replaceCurrentHitWith(targetPrimitive);
	probe.getCurrentHit()->calcIntersectionDetail(ray, probe, out_detail);
}

bool SuperpositionedPrimitive::mayIntersectVolume(const math::AABB3D& aabb) const
{
	PH_ASSERT(m_mainPrimitive);

	return m_mainPrimitive->mayIntersectVolume(aabb);
}

math::AABB3D SuperpositionedPrimitive::calcAABB() const
{
	PH_ASSERT(m_mainPrimitive);

	return m_mainPrimitive->calcAABB();
}

real SuperpositionedPrimitive::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	PH_ASSERT(m_mainPrimitive);

	return m_mainPrimitive->calcPositionSamplePdfA(position);
}

real SuperpositionedPrimitive::calcExtendedArea() const
{
	PH_ASSERT(m_mainPrimitive);

	return m_mainPrimitive->calcExtendedArea();
}

void SuperpositionedPrimitive::genPositionSample(SampleFlow& sampleFlow, PositionSample* const out_sample) const
{
	PH_ASSERT(m_mainPrimitive);

	m_mainPrimitive->genPositionSample(sampleFlow, out_sample);
}

}// end namespace ph
