#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector3.h"

namespace ph
{

const StaticAffineTransform& StaticAffineTransform::makeIdentity()
{
	static const StaticAffineTransform identityTransform(math::Matrix4R::makeIdentity(), math::Matrix4R::makeIdentity());
	return identityTransform;
}

StaticAffineTransform::StaticAffineTransform() :
	StaticAffineTransform(math::Matrix4R::makeIdentity(), math::Matrix4R::makeIdentity())
{}

StaticAffineTransform::StaticAffineTransform(const math::Matrix4R& transform, const math::Matrix4R& inverseTransform) :
	m_transformMatrix(transform), m_inverseTransformMatrix(inverseTransform)
{
	// TODO: assert on actually affine
}

std::unique_ptr<Transform> StaticAffineTransform::genInversed() const
{
	auto inversed = std::make_unique<StaticAffineTransform>();
	inversed->m_transformMatrix        = m_inverseTransformMatrix;
	inversed->m_inverseTransformMatrix = m_transformMatrix;

	return std::move(inversed);
}

void StaticAffineTransform::transformVector(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	// TODO: use quaternion
	m_transformMatrix.mul(vector, 0.0_r, out_vector);
}

void StaticAffineTransform::transformOrientation(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	const math::Matrix4R transposedInverse = m_inverseTransformMatrix.transpose();
	transposedInverse.mul(orientation, 0.0_r, out_orientation);
}

void StaticAffineTransform::transformPoint(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	m_transformMatrix.mul(point, 1.0_r, out_point);
}

void StaticAffineTransform::transformLineSegment(
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
	StaticAffineTransform::transformPoint (lineStartPos, time, out_lineStartPos);
	StaticAffineTransform::transformVector(lineDir,      time, out_lineDir);
	*out_lineMinT = lineMinT;
	*out_lineMaxT = lineMaxT;
}

}// end namespace ph
