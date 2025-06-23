#include "Engine/Core/Transform/StaticRigidTransform.h"

namespace ph
{

const StaticRigidTransform& StaticRigidTransform::makeIdentity()
{
	static const StaticRigidTransform identityTransform(StaticAffineTransform::makeIdentity());
	return identityTransform;
}

StaticRigidTransform::StaticRigidTransform() : 
	m_staticTransform()
{}

StaticRigidTransform::StaticRigidTransform(const StaticAffineTransform& transform) :
	m_staticTransform(transform)
{
	// TODO: assert on scale-free
}

void StaticRigidTransform::transformVector(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	m_staticTransform.transformVector(vector, time, out_vector);
}

void StaticRigidTransform::transformOrientation(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	m_staticTransform.transformOrientation(orientation, time, out_orientation);
}

void StaticRigidTransform::transformPoint(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	m_staticTransform.transformPoint(point, time, out_point);
}

void StaticRigidTransform::transformLineSegment(
	const math::TLineSegment<real>& segment,
	const Time&                     time,
	math::TLineSegment<real>* const out_segment) const
{
	m_staticTransform.transformLineSegment(
		segment,
		time, 
		out_segment);
}

}// end namespace ph
