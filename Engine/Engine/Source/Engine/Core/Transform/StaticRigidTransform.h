#pragma once

#include "Engine/Core/Transform/RigidTransform.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Math/TDecomposedTransform.h"

#include <Common/assertion.h>
#include <Common/logging.h>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(StaticRigidTransform, Math);

/*!
A static transform that enforces object rigidity during the transformation process.
*/
class StaticRigidTransform : public RigidTransform
{
public:
	static const StaticRigidTransform& makeIdentity();

	template<typename U>
	static StaticRigidTransform makeForward(const math::TDecomposedTransform<U>& transform);

	template<typename U>
	static StaticRigidTransform makeInverse(const math::TDecomposedTransform<U>& transform);

	template<typename U>
	static StaticRigidTransform makeParentedForward(const std::vector<math::TDecomposedTransform<U>>& fromRootToLocal);

	template<typename U>
	static StaticRigidTransform makeParentedInverse(const std::vector<math::TDecomposedTransform<U>>& fromRootToLocal);

public:
	/*! @brief Creates an identity transform.
	*/
	StaticRigidTransform();

private:
	void transformVector(
		const math::Vector3R& vector,
		const Time&           time,
		math::Vector3R*       out_vector) const override;

	void transformOrientation(
		const math::Vector3R& orientation,
		const Time&           time,
		math::Vector3R*       out_orientation) const override;

	void transformPoint(
		const math::Vector3R& point,
		const Time&           time,
		math::Vector3R*       out_point) const override;

	void transformLineSegment(
		const math::Vector3R& lineStartPos,
		const math::Vector3R& lineDir,
		real                  lineMinT,
		real                  lineMaxT,
		const Time&           time,
		math::Vector3R*       out_lineStartPos,
		math::Vector3R*       out_lineDir,
		real*                 out_lineMinT,
		real*                 out_lineMaxT) const override;

private:
	StaticAffineTransform m_staticTransform;

	explicit StaticRigidTransform(const StaticAffineTransform& transform);

	template<typename U>
	static inline std::vector<math::TDecomposedTransform<U>> getScaleFreeTransforms(
		const std::vector<math::TDecomposedTransform<U>>& transforms);
};

template<typename U>
inline auto StaticRigidTransform::makeForward(const math::TDecomposedTransform<U>& transform)
	-> StaticRigidTransform
{
	return StaticRigidTransform(StaticAffineTransform::makeForward(getScaleFreeTransforms<U>({transform})[0]));
}

template<typename U>
inline auto StaticRigidTransform::makeInverse(const math::TDecomposedTransform<U>& transform)
	-> StaticRigidTransform
{
	return StaticRigidTransform(StaticAffineTransform::makeInverse(getScaleFreeTransforms<U>({transform})[0]));
}

template<typename U>
inline auto StaticRigidTransform::makeParentedForward(const std::vector<math::TDecomposedTransform<U>>& fromRootToLocal)
	-> StaticRigidTransform
{
	return StaticRigidTransform(StaticAffineTransform::makeParentedForward(getScaleFreeTransforms<U>({fromRootToLocal})));
}

template<typename U>
inline auto StaticRigidTransform::makeParentedInverse(const std::vector<math::TDecomposedTransform<U>>& fromRootToLocal)
	-> StaticRigidTransform
{
	return StaticRigidTransform(StaticAffineTransform::makeParentedInverse(getScaleFreeTransforms<U>({fromRootToLocal})));
}

template<typename U>
inline auto StaticRigidTransform::getScaleFreeTransforms(const std::vector<math::TDecomposedTransform<U>>& transforms)
	-> std::vector<math::TDecomposedTransform<U>>
{
	std::vector<math::TDecomposedTransform<U>> scaleFreeTransforms;
	for(const auto& transform : transforms)
	{
		// FIXME: better scale tolerance value, not hardcoded like this
		if(!transform.hasScaleEffect(0.000001_r))
		{
			scaleFreeTransforms.push_back(transform);
		}
		else
		{
			PH_LOG(StaticRigidTransform, Warning,
				"scale effect detected, which is {}, ignoring", transform.getScale().toString());

			scaleFreeTransforms.push_back(math::TDecomposedTransform<U>(transform).setScale(1));
		}
	}

	PH_ASSERT(scaleFreeTransforms.size() == transforms.size());

	return scaleFreeTransforms;
}

}// end namespace ph::math
