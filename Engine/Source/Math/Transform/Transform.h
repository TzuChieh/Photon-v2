#pragma once

#include "Math/math_fwd.h"

namespace ph
{

class Ray;

class Transform
{
public:
	virtual ~Transform() = 0;

	// Treating a Vector3R as either a vector, normal, or point and calculate the transformed result.
	virtual void transformVector(const Vector3R& vector, Vector3R* const out_transformedVector) const = 0;
	//virtual void transformNormal(const Vector3R& normal, Vector3R* const out_transformedNormal) const = 0;
	virtual void transformPoint(const Vector3R& point, Vector3R* const out_transformedPoint) const = 0;

	// Notice that transforming a ray neither will change its parametric length (t) nor renormalizing its direction
	// vector even if the transform contains scale factor; because if users respect the ray segment's definition:
	// 
	//     Ray Segment = [rayOrigin + rayMinT * rayDirection, rayOrigin + rayMaxT * rayDirection]
	// 
	// this operation will always yield a correctly transformed result while saving an expensive sqrt() call.
	virtual void transformRay(const Ray& ray, Ray* const out_transformedRay) const = 0;
};

}// end namespace ph