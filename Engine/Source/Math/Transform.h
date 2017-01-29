#pragma once

#include "Math/Matrix4f.h"

namespace ph
{

class Vector3f;
class AABB;
class Ray;

class Transform final
{
public:
	Transform();
	Transform(const Matrix4f& transformMatrix);

	void transformVector(const Vector3f& vector, Vector3f* const out_transformedVector) const;
	void transformPoint(const Vector3f& point, Vector3f* const out_transformedPoint) const;

	// Notice that transforming a ray will not change its parametric length (t) nor renormalizing its direction
	// vector even if the transform contains scale factor; because when we use "rayDirection * t" to obtain the 
	// ray's endpoint coordinate, this configuration will still yield a correctly transformed result while saving
	// an expensive sqrt() call.
	void transformRay(const Ray& ray, Ray* const out_transformedRay) const;

	// Normalized version of transformRay().
	// (ray direction vector is normalized, and parametric length (t) also changed accordingly)
	void transformRayNormalized(const Ray& ray, Ray* const out_transformedRay) const;

	void transform(const AABB& aabb, AABB* const out_aabb) const;
	Transform transform(const Transform& rhs) const;

private:
	Matrix4f m_transformMatrix;
};

}// end namespace ph