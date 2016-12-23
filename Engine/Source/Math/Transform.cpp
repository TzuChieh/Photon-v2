#include "Math/Transform.h"
#include "Core/BoundingVolume/AABB.h"
#include "Core/Ray.h"

#include <iostream>

namespace ph
{

const Transform Transform::EMPTY_TRANSFORM;

Transform::Transform()
{
	m_transformMatrix.initIdentity();
}

Transform::Transform(const Matrix4f& transformMatrix) : 
	m_transformMatrix(transformMatrix)
{

}

void Transform::transformVector(const Vector3f& vector, Vector3f* const out_transformedVector) const
{
	// TODO: use quaternion
	m_transformMatrix.mul(vector, 0.0f, out_transformedVector);
}

void Transform::transformPoint(const Vector3f& point, Vector3f* const out_transformedPoint) const
{
	m_transformMatrix.mul(point, 1.0f, out_transformedPoint);
}

void Transform::transformRay(const Ray& ray, Ray* const out_transformedRay) const
{
	m_transformMatrix.mul(ray.getOrigin(), 1.0f, &(out_transformedRay->getOrigin()));
	m_transformMatrix.mul(ray.getDirection(), 0.0f, &(out_transformedRay->getDirection()));
	out_transformedRay->setMinT(ray.getMinT());
	out_transformedRay->setMaxT(ray.getMaxT());
}

void Transform::transformRayNormalized(const Ray& ray, Ray* const out_transformedRay) const
{
	m_transformMatrix.mul(ray.getOrigin(), 1.0f, &(out_transformedRay->getOrigin()));
	m_transformMatrix.mul(ray.getDirection(), 0.0f, &(out_transformedRay->getDirection()));

	// normalizing the ray
	const float32 dirLength = out_transformedRay->getDirection().length();
	out_transformedRay->setMinT(ray.getMinT() * dirLength);
	out_transformedRay->setMaxT(ray.getMaxT() * dirLength);
	out_transformedRay->getDirection().mulLocal(1.0f / dirLength);
}

void Transform::transform(const AABB& aabb, AABB* const out_aabb) const
{
	// TODO: below is wrong

	std::cerr << "transforming AABB is not implemented" << std::endl;
}

Transform Transform::transform(const Transform& rhs) const
{
	return Transform(m_transformMatrix.mul(rhs.m_transformMatrix));
}

}// end namespace ph