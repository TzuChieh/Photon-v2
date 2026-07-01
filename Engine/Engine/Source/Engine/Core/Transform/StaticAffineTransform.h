#pragma once

#include "Engine/Math/TMatrix4.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/TDecomposedTransform.h"
#include "Engine/Core/Transform/Transform.h"
#include "Engine/Utility/TSpan.h"

#include <vector>

namespace ph
{

class StaticAffineTransform : public Transform
{
	friend class StaticRigidTransform;

public:
	static const StaticAffineTransform& makeIdentity();

	/*!
	Generates a StaticAffineTransform that can be used to transform values from local to world space.
	*/
	template<typename U>
	static StaticAffineTransform makeForward(const math::TDecomposedTransform<U>& transform);

	/*!
	Generates a StaticAffineTransform that can be used to transform values from world to local space.
	*/
	template<typename U>
	static StaticAffineTransform makeInverse(const math::TDecomposedTransform<U>& transform);

	/*!
	Given a chain of transforms from root (world, the last transform applied) to local (the first
	transform applied), generates a `StaticAffineTransform` that can be used to transform values
	from local to world space.
	*/
	template<typename U>
	static StaticAffineTransform makeParentedForward(TSpanView<math::TDecomposedTransform<U>> fromRootToLocal);

	/*!
	Given a chain of transforms from root (world, the last transform applied) to local (the first
	transform applied), generates a `StaticAffineTransform` that can be used to transform values
	from world to local space.
	*/
	template<typename U>
	static StaticAffineTransform makeParentedInverse(TSpanView<math::TDecomposedTransform<U>> fromRootToLocal);

public:
	/*! @brief Creates an identity transform.
	*/
	StaticAffineTransform();

	StaticAffineTransform(const math::Matrix4R& transform, const math::Matrix4R& inverseTransform);

	std::unique_ptr<Transform> genInversed() const override;

	const math::Matrix4R& getTransformMatrix() const;
	const math::Matrix4R& getInversedTransformMatrix() const;

private:
	void doTransformRay(
		const Ray& ray,
		Ray*       out_ray) const override;

	void doTransformHitInfo(
		const HitInfo& info,
		const Time&    time,
		HitInfo*       out_info) const override;

	void doTransformVector(
		const math::Vector3R& vector,
		const Time&           time,
		math::Vector3R*       out_vector) const override;

	void doTransformOrientation(
		const math::Vector3R& orientation,
		const Time&           time,
		math::Vector3R*       out_orientation) const override;

	void doTransformPoint(
		const math::Vector3R& point,
		const Time&           time,
		math::Vector3R*       out_point) const override;

	void doTransformLineSegment(
		const math::TLineSegment<real>& segment,
		const Time&                     time,
		math::TLineSegment<real>*       out_segment) const override;

private:
	math::Matrix4R m_transformMatrix;
	math::Matrix4R m_inverseTransformMatrix;
};

template<typename U>
inline StaticAffineTransform StaticAffineTransform
::makeForward(const math::TDecomposedTransform<U>& transform)
{
	math::TMatrix4<U> transformMatrix;
	math::TMatrix4<U> invTransformMatrix;
	transform.genTransformMatrix(&transformMatrix);
	transform.genInverseTransformMatrix(&invTransformMatrix);

	return StaticAffineTransform(math::Matrix4R(transformMatrix), math::Matrix4R(invTransformMatrix));
}

template<typename U>
inline StaticAffineTransform StaticAffineTransform
::makeInverse(const math::TDecomposedTransform<U>& transform)
{
	math::TMatrix4<U> transformMatrix;
	math::TMatrix4<U> invTransformMatrix;
	transform.genInverseTransformMatrix(&transformMatrix);
	transform.genTransformMatrix(&invTransformMatrix);

	return StaticAffineTransform(math::Matrix4R(transformMatrix), math::Matrix4R(invTransformMatrix));
}

template<typename U>
inline StaticAffineTransform StaticAffineTransform
::makeParentedForward(TSpanView<math::TDecomposedTransform<U>> fromRootToLocal)
{
	math::TMatrix4<U> parentedMatrix(math::TMatrix4<U>::makeIdentity());
	math::TMatrix4<U> invParentedMatrix(math::TMatrix4<U>::makeIdentity());
	for(const auto& transform : fromRootToLocal)
	{
		math::TMatrix4<U> matrix;
		math::TMatrix4<U> invMatrix;
		transform.genTransformMatrix(&matrix);
		transform.genInverseTransformMatrix(&invMatrix);

		parentedMatrix    = parentedMatrix.mul(matrix);
		invParentedMatrix = invMatrix.mul(invParentedMatrix);
	}

	return StaticAffineTransform(math::Matrix4R(parentedMatrix), math::Matrix4R(invParentedMatrix));
}

template<typename U>
inline StaticAffineTransform StaticAffineTransform
::makeParentedInverse(TSpanView<math::TDecomposedTransform<U>> fromRootToLocal)
{
	math::TMatrix4<U> parentedMatrix(math::TMatrix4<U>::makeIdentity());
	math::TMatrix4<U> invParentedMatrix(math::TMatrix4<U>::makeIdentity());
	for(const auto& transform : fromRootToLocal)
	{
		math::TMatrix4<U> matrix;
		math::TMatrix4<U> invMatrix;
		transform.genInverseTransformMatrix(&matrix);
		transform.genTransformMatrix(&invMatrix);

		parentedMatrix    = matrix.mul(parentedMatrix);
		invParentedMatrix = invParentedMatrix.mul(invMatrix);
	}

	return StaticAffineTransform(Matrix4R(parentedMatrix), Matrix4R(invParentedMatrix));
}

inline const math::Matrix4R& StaticAffineTransform
::getTransformMatrix() const
{
	return m_transformMatrix;
}

inline const math::Matrix4R& StaticAffineTransform
::getInversedTransformMatrix() const
{
	return m_inverseTransformMatrix;
}

inline void StaticAffineTransform::doTransformVector(
	const math::Vector3R& vector,
	const Time&,
	math::Vector3R* const out_vector) const
{
	// TODO: use quaternion
	m_transformMatrix.mul(vector, out_vector);
}

inline void StaticAffineTransform::doTransformOrientation(
	const math::Vector3R& orientation,
	const Time&,
	math::Vector3R* const out_orientation) const
{
	m_inverseTransformMatrix.transposeMul(orientation, out_orientation);
}

inline void StaticAffineTransform::doTransformPoint(
	const math::Vector3R& point,
	const Time&,
	math::Vector3R* const out_point) const
{
	m_transformMatrix.mul(point, 1.0_r, out_point);
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
