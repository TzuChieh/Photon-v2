#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"

#include <iostream>

namespace ph::math
{

const StaticAffineTransform& StaticAffineTransform::IDENTITY()
{
	static const StaticAffineTransform identityTransform(Matrix4R::IDENTITY(), Matrix4R::IDENTITY());
	return identityTransform;
}

StaticAffineTransform::StaticAffineTransform() :
	StaticAffineTransform(Matrix4R::IDENTITY(), Matrix4R::IDENTITY())
{}

StaticAffineTransform::StaticAffineTransform(const Matrix4R& transform, const Matrix4R& inverseTransform) :
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
	const Vector3R& vector, 
	const Time&     time,
	Vector3R* const out_vector) const
{
	// TODO: use quaternion
	m_transformMatrix.mul(vector, 0.0_r, out_vector);
}

void StaticAffineTransform::transformOrientation(
	const Vector3R& orientation, 
	const Time&     time,
	Vector3R* const out_orientation) const
{
	const Matrix4R transposedInverse = m_inverseTransformMatrix.transpose();
	transposedInverse.mul(orientation, 0.0_r, out_orientation);
}

void StaticAffineTransform::transformPoint(
	const Vector3R& point, 
	const Time&     time,
	Vector3R* const out_point) const
{
	m_transformMatrix.mul(point, 1.0_r, out_point);
}

void StaticAffineTransform::transformLineSegment(
	const Vector3R& lineStartPos, 
	const Vector3R& lineDir,
	const real      lineMinT, 
	const real      lineMaxT, 
	const Time&     time,
	Vector3R* const out_lineStartPos,
	Vector3R* const out_lineDir,
	real* const     out_lineMinT, 
	real* const     out_lineMaxT) const
{
	StaticAffineTransform::transformPoint (lineStartPos, time, out_lineStartPos);
	StaticAffineTransform::transformVector(lineDir,      time, out_lineDir);
	*out_lineMinT = lineMinT;
	*out_lineMaxT = lineMaxT;
}

}// end namespace ph::math
