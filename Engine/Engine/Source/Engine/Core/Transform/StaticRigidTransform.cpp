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
	const math::Vector3R& lineStartPos,
	const math::Vector3R& lineDir,
	const real            lineMinT,
	const real            lineMaxT,
	const Time&           time,
	math::Vector3R*       out_lineStartPos,
	math::Vector3R*       out_lineDir,
	real* const           out_lineMinT,
	real* const           out_lineMaxT) const
{
	m_staticTransform.transformLineSegment(
		lineStartPos, 
		lineDir, 
		lineMinT, 
		lineMaxT, 
		time, 
		out_lineStartPos, 
		out_lineDir, 
		out_lineMinT, 
		out_lineMaxT);
}

}// end namespace ph
