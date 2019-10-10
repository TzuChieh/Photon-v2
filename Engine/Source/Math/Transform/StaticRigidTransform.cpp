#include "Math/Transform/StaticRigidTransform.h"

namespace ph::math
{

const Logger StaticRigidTransform::logger(LogSender("StaticRigidTransform"));

StaticRigidTransform StaticRigidTransform::makeIdentity()
{
	return StaticRigidTransform(StaticAffineTransform::IDENTITY());
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
	const Vector3R& vector,
	const Time&     time,
	Vector3R* const out_vector) const
{
	m_staticTransform.transformVector(vector, time, out_vector);
}

void StaticRigidTransform::transformOrientation(
	const Vector3R& orientation,
	const Time&     time,
	Vector3R* const out_orientation) const
{
	m_staticTransform.transformOrientation(orientation, time, out_orientation);
}

void StaticRigidTransform::transformPoint(
	const Vector3R& point,
	const Time&     time,
	Vector3R* const out_point) const
{
	m_staticTransform.transformPoint(point, time, out_point);
}

void StaticRigidTransform::transformLineSegment(
	const Vector3R& lineStartPos,
	const Vector3R& lineDir,
	const real      lineMinT,
	const real      lineMaxT,
	const Time&     time,
	Vector3R*       out_lineStartPos,
	Vector3R*       out_lineDir,
	real* const     out_lineMinT,
	real* const     out_lineMaxT) const
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

}// end namespace ph::math
