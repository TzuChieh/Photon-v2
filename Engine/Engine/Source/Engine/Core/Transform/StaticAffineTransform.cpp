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
	const math::TLineSegment<real>& segment,
	const Time&                     time,
	math::TLineSegment<real>* const out_segment) const
{
	math::Vector3R tOrigin;
	StaticAffineTransform::transformPoint(segment.getOrigin(), time, &tOrigin);

	math::Vector3R tDir;
	StaticAffineTransform::transformVector(segment.getDir(), time, &tDir);

	out_segment->setOrigin(tOrigin);
	out_segment->setDir(tDir);
	out_segment->setRange(segment.getMinT(), segment.getMaxT());
}

}// end namespace ph
