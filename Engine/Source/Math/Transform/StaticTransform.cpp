#include "Math/Transform/StaticTransform.h"
#include "Core/BoundingVolume/AABB.h"
#include "Core/Ray.h"

#include <iostream>

namespace ph
{

StaticTransform::StaticTransform() : 
	StaticTransform(Matrix4R::IDENTITY(), Matrix4R::IDENTITY())
{

}

StaticTransform::StaticTransform(const Matrix4R& transform, const Matrix4R& inverseTransform) : 
	m_transformMatrix(transform), m_inverseTransformMatrix(inverseTransform)
{

}

StaticTransform::~StaticTransform() = default;

void StaticTransform::transformVector(const Vector3R& vector, Vector3R* const out_transformedVector) const
{
	// TODO: use quaternion
	m_transformMatrix.mul(vector, 0.0_r, out_transformedVector);
}

void StaticTransform::transformPoint(const Vector3R& point, Vector3R* const out_transformedPoint) const
{
	m_transformMatrix.mul(point, 1.0_r, out_transformedPoint);
}

void StaticTransform::transformRay(const Ray& ray, Ray* const out_transformedRay) const
{
	m_transformMatrix.mul(ray.getOrigin(), 1.0_r, &(out_transformedRay->getOrigin()));
	m_transformMatrix.mul(ray.getDirection(), 0.0_r, &(out_transformedRay->getDirection()));
	out_transformedRay->setMinT(ray.getMinT());
	out_transformedRay->setMaxT(ray.getMaxT());
}

void StaticTransform::transformVector(const Vector3R& vector,
	const Time& time,
	Vector3R* out_vector) const
{

}

void StaticTransform::transformOrientation(const Vector3R& orientation,
	const Time& time,
	Vector3R* out_orientation) const
{

}

void StaticTransform::transformPoint(const Vector3R& point,
	const Time& time,
	Vector3R* out_point) const
{

}

void StaticTransform::transformLineSegment(const Vector3R& lineStartPos,
	const Vector3R& lineDir,
	real lineMinT, real lineMaxT,
	const Time& time,
	Vector3R* out_lineStartPos,
	Vector3R* out_lineDir) const
{

}

}// end namespace ph