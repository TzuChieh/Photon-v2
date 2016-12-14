#include "Math/Transform.h"
#include "Entity/BoundingVolume/AABB.h"

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