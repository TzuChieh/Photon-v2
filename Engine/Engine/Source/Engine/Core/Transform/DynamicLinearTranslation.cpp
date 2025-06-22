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

void DynamicLinearTranslation::transformVector(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	*out_vector = vector;
}

void DynamicLinearTranslation::transformOrientation(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	*out_orientation = orientation;
}

void DynamicLinearTranslation::transformPoint(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	*out_point = point.add(math::Vector3R::lerp(m_translationT0, m_translationT1, time.relativeT));
}

void DynamicLinearTranslation::transformLineSegment(
	const math::Vector3R& lineStartPos,
	const math::Vector3R& lineDir,
	const real            lineMinT, 
	const real            lineMaxT,
	const Time&           time,
	math::Vector3R* const out_lineStartPos,
	math::Vector3R* const out_lineDir,
	real* const           out_lineMinT, 
	real* const           out_lineMaxT) const
{
	DynamicLinearTranslation::transformPoint(lineStartPos, time, out_lineStartPos);
	*out_lineDir  = lineDir;
	*out_lineMinT = lineMinT;
	*out_lineMaxT = lineMaxT;
}

}// end namespace ph
