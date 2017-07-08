#pragma once

#include "Math/TMatrix4.h"
#include "Math/TVector3.h"
#include "Math/Transform/Transform.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <vector>

namespace ph
{

class AABB;
class Ray;

class StaticTransform final : public Transform
{
public:
	template<typename U>
	static StaticTransform makeForward(const TDecomposedTransform<U>& transform);

	template<typename U>
	static StaticTransform makeInverse(const TDecomposedTransform<U>& transform);

	template<typename U>
	static StaticTransform makeParentedForward(const std::vector<TDecomposedTransform<U>>& fromRootToLocal);

	template<typename U>
	static StaticTransform makeParentedInverse(const std::vector<TDecomposedTransform<U>>& fromRootToLocal);

public:
	StaticTransform();
	StaticTransform(const Matrix4R& transform, const Matrix4R& inverseTransform);
	virtual ~StaticTransform() override;

	virtual void transformVector(const Vector3R& vector, Vector3R* out_transformedVector) const override;
	//virtual void transformNormal(const Vector3R& normal, Vector3R* const out_transformedNormal) const override;
	virtual void transformPoint(const Vector3R& point, Vector3R* out_transformedPoint) const override;

	// Notice that transforming a ray will not change its parametric length (t) nor renormalizing its direction
	// vector even if the transform contains scale factor; because when we use "rayDirection * t" to obtain the 
	// ray's endpoint coordinate, this configuration will still yield a correctly transformed result while saving
	// an expensive sqrt() call.
	virtual void transformRay(const Ray& ray, Ray* out_transformedRay) const override;

private:
	virtual void transformVector(const Vector3R& vector, const Time& time, 
	                             Vector3R* out_vector) const override;

	virtual void transformOrientation(const Vector3R& orientation, const Time& time,
	                                  Vector3R* out_orientation) const override;

	virtual void transformPoint(const Vector3R& point, const Time& time,
	                            Vector3R* out_point) const override;

	virtual void transformLineSegment(const Vector3R& lineStartPos, const Vector3R& lineDir,
	                                  real lineMinT, real lineMaxT, const Time& time,
	                                  Vector3R* out_lineStartPos, Vector3R* out_lineDir,
	                                  real* out_lineMinT, real* out_lineMaxT) const override;

private:
	Matrix4R m_transformMatrix;
	Matrix4R m_inverseTransformMatrix;
};

// Implementation:

template<typename U>
StaticTransform StaticTransform::makeForward(const TDecomposedTransform<U>& transform)
{
	TMatrix4<U> transformMatrix;
	TMatrix4<U> invTransformMatrix;
	transform.genTransformMatrix(&transformMatrix);
	transform.genInverseTransformMatrix(&invTransformMatrix);

	return StaticTransform(Matrix4R(transformMatrix), Matrix4R(invTransformMatrix));
}

template<typename U>
StaticTransform StaticTransform::makeInverse(const TDecomposedTransform<U>& transform)
{
	TMatrix4<U> transformMatrix;
	TMatrix4<U> invTransformMatrix;
	transform.genInverseTransformMatrix(&transformMatrix);
	transform.genTransformMatrix(&invTransformMatrix);

	return StaticTransform(Matrix4R(transformMatrix), Matrix4R(invTransformMatrix));
}

template<typename U>
StaticTransform StaticTransform::makeParentedForward(const std::vector<TDecomposedTransform<U>>& fromRootToLocal)
{
	TMatrix4<U> parentedMatrix(TMatrix4<U>::IDENTITY());
	TMatrix4<U> invParentedMatrix(TMatrix4<U>::IDENTITY());
	for(const auto& transform : fromRootToLocal)
	{
		TMatrix4<U> matrix;
		TMatrix4<U> invMatrix;
		transform.genTransformMatrix(&matrix);
		transform.genInverseTransformMatrix(&invMatrix);

		parentedMatrix    = parentedMatrix.mul(matrix);
		invParentedMatrix = invMatrix.mul(invParentedMatrix);
	}

	return StaticTransform(Matrix4R(parentedMatrix), Matrix4R(invParentedMatrix));
}

template<typename U>
StaticTransform StaticTransform::makeParentedInverse(const std::vector<TDecomposedTransform<U>>& fromRootToLocal)
{
	TMatrix4<U> parentedMatrix(TMatrix4<U>::IDENTITY());
	TMatrix4<U> invParentedMatrix(TMatrix4<U>::IDENTITY());
	for(const auto& transform : fromRootToLocal)
	{
		TMatrix4<U> matrix;
		TMatrix4<U> invMatrix;
		transform.genInverseTransformMatrix(&matrix);
		transform.genTransformMatrix(&invMatrix);

		parentedMatrix    = matrix.mul(parentedMatrix);
		invParentedMatrix = invParentedMatrix.mul(invMatrix);
	}

	return StaticTransform(Matrix4R(parentedMatrix), Matrix4R(invParentedMatrix));
}

// FIXME: precision loss in parent (it is using real number)
//template<typename U>
//StaticTransform::StaticTransform(const StaticTransform& parent, const TDecomposedTransform<U>& local) : 
//	StaticTransform()
//{
//	const StaticTransform localTransform(local);
//
//	m_transformMatrix        = parent.m_transformMatrix.mul(localTransform.m_transformMatrix);
//	m_inverseTransformMatrix = localTransform.m_inverseTransformMatrix(parent.m_inverseTransformMatrix);
//}

}// end namespace ph