#include "Engine/Core/Transform/DynamicLinearTranslation.h"
#include "Engine/Core/Quantity/Time.h"

namespace ph
{

DynamicLinearTranslation::DynamicLinearTranslation(const math::Vector3R& translationT0,
                                                   const math::Vector3R& translationT1) :
	m_translationT0(translationT0), 
	m_translationT1(translationT1)
{}

DynamicLinearTranslation DynamicLinearTranslation::makeInversed() const
{
	return DynamicLinearTranslation(m_translationT0.mul(-1), m_translationT1.mul(-1));
}

std::unique_ptr<Transform> DynamicLinearTranslation::genInversed() const
{
	return std::make_unique<DynamicLinearTranslation>(m_translationT0.mul(-1),
	                                                  m_translationT1.mul(-1));
}

bool DynamicLinearTranslation::hasMotion(const Time& startTime, const Time& endTime) const
{
	if(startTime.getStepT() == endTime.getStepT())
	{
		return false;
	}
	else if(m_translationT0.isEqual(m_translationT1))
	{
		return false;
	}

	return true;
}

void DynamicLinearTranslation::doCalcSweepAABB(
	const math::AABB3D& aabb,
	const Time&         startTime,
	const Time&         endTime,
	const uint32        /* numSamples */,
	math::AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	math::AABB3D startAABB;
	transform(aabb, startTime, &startAABB);

	math::AABB3D endAABB;
	transform(aabb, endTime, &endAABB);

	startAABB.unionWith(endAABB);
	*out_aabb = startAABB;
}

void DynamicLinearTranslation::doTransformVector(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	*out_vector = vector;
}

void DynamicLinearTranslation::doTransformOrientation(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	*out_orientation = orientation;
}

void DynamicLinearTranslation::doTransformPoint(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	*out_point = point.add(math::Vector3R::lerp(m_translationT0, m_translationT1, time.getStepT()));
}

void DynamicLinearTranslation::doTransformLineSegment(
	const math::TLineSegment<real>& segment,
	const Time&                     time,
	math::TLineSegment<real>* const out_segment) const
{
	math::Vector3R tOrigin;
	DynamicLinearTranslation::doTransformPoint(segment.getOrigin(), time, &tOrigin);

	*out_segment = segment;
	out_segment->setOrigin(tOrigin);
}

}// end namespace ph
