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

bool SuperpositionedPrimitive::isIntersecting(const Ray& ray) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	return m_mainPrimitive->isIntersecting(ray);
}

bool SuperpositionedPrimitive::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

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

bool SuperpositionedPrimitive::isIntersectingVolumeConservative(const AABB3D& aabb) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	return m_mainPrimitive->isIntersectingVolumeConservative(aabb);
}

void SuperpositionedPrimitive::calcAABB(AABB3D* const out_aabb) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	m_mainPrimitive->calcAABB(out_aabb);
}

real SuperpositionedPrimitive::calcPositionSamplePdfA(const Vector3R& position) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	return m_mainPrimitive->calcPositionSamplePdfA(position);
}

real SuperpositionedPrimitive::calcExtendedArea() const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	return m_mainPrimitive->calcExtendedArea();
}

void SuperpositionedPrimitive::genPositionSample(PositionSample* const out_sample) const
{
	PH_ASSERT(m_mainPrimitive != nullptr);

	m_mainPrimitive->genPositionSample(out_sample);
}

}// end namespace ph
