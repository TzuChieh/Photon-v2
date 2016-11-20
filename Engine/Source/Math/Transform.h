#pragma once

#include "Math/Matrix4f.h"

namespace ph
{

class AABB;

class Transform final
{
public:
	Transform();
	Transform(const Matrix4f& transformMatrix);

	void transformVector(const Vector3f& vector, Vector3f* const out_transformedVector) const;
	void transformPoint(const Vector3f& point, Vector3f* const out_transformedPoint) const;
	void transform(const AABB& aabb, AABB* const out_aabb) const;

private:
	Matrix4f m_transformMatrix;
};

}// end namespace ph