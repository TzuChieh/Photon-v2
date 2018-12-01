#pragma once

#include "Math/TMatrix4.h"
#include "Math/TVector3.h"
#include "Math/Transform/Transform.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <vector>

namespace ph
{
class Ray;

class StaticAffineTransform final : public Transform
{
	friend class StaticRigidTransform;

public:
	static const StaticAffineTransform& IDENTITY();

	// Generates a StaticAffineTransform that can be used to transform values from 
	// local to world space.
	//
	template<typename U>
	static StaticAffineTransform makeForward(const TDecomposedTransform<U>& transform);

	// Generates a StaticAffineTransform that can be used to transform values from 
	// world to local space.
	//
	template<typename U>
	static StaticAffineTransform makeInverse(const TDecomposedTransform<U>& transform);

	// Given a chain of transforms from root (world) to local, generates a 
	// StaticAffineTransform that can be used to transform values from local to world space.
	//
	template<typename U>
	static StaticAffineTransform makeParentedForward(const std::vector<TDecomposedTransform<U>>& fromRootToLocal);

	// Given a chain of transforms from root (world) to local, generates a 
	// StaticAffineTransform that can be used to transform values from world to local space.
	//
	template<typename U>
	static StaticAffineTransform makeParentedInverse(const std::vector<TDecomposedTransform<U>>& fromRootToLocal);

public:
	StaticAffineTransform();
	StaticAffineTransform(const Matrix4R& transform, const Matrix4R& inverseTransform);
	virtual ~StaticAffineTransform() override;

	virtual std::unique_ptr<Transform> genInversed() const override;

private:
	virtual void transformVector(
		const Vector3R& vector,
		const Time&     time,
		Vector3R*       out_vector) const override;

	virtual void transformOrientation(
		const Vector3R& orientation,
		const Time&     time,
		Vector3R*       out_orientation) const override;

	virtual void transformPoint(
		const Vector3R& point,
		const Time&     time,
		Vector3R*       out_point) const override;

	virtual void transformLineSegment(
		const Vector3R& lineStartPos,
		const Vector3R& lineDir,
		real            lineMinT,
		real            lineMaxT,
		const Time&     time,
		Vector3R*       out_lineStartPos,
		Vector3R*       out_lineDir,
		real*           out_lineMinT,
		real*           out_lineMaxT) const override;

private:
	Matrix4R m_transformMatrix;
	Matrix4R m_inverseTransformMatrix;
};

// Implementation:

template<typename U>
StaticAffineTransform StaticAffineTransform::makeForward(const TDecomposedTransform<U>& transform)
{
	TMatrix4<U> transformMatrix;
	TMatrix4<U> invTransformMatrix;
	transform.genTransformMatrix(&transformMatrix);
	transform.genInverseTransformMatrix(&invTransformMatrix);

	return StaticAffineTransform(Matrix4R(transformMatrix), Matrix4R(invTransformMatrix));
}

template<typename U>
StaticAffineTransform StaticAffineTransform::makeInverse(const TDecomposedTransform<U>& transform)
{
	TMatrix4<U> transformMatrix;
	TMatrix4<U> invTransformMatrix;
	transform.genInverseTransformMatrix(&transformMatrix);
	transform.genTransformMatrix(&invTransformMatrix);

	return StaticAffineTransform(Matrix4R(transformMatrix), Matrix4R(invTransformMatrix));
}

template<typename U>
StaticAffineTransform StaticAffineTransform::makeParentedForward(const std::vector<TDecomposedTransform<U>>& fromRootToLocal)
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

	return StaticAffineTransform(Matrix4R(parentedMatrix), Matrix4R(invParentedMatrix));
}

template<typename U>
StaticAffineTransform StaticAffineTransform::makeParentedInverse(const std::vector<TDecomposedTransform<U>>& fromRootToLocal)
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

	return StaticAffineTransform(Matrix4R(parentedMatrix), Matrix4R(invParentedMatrix));
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