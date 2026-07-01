#pragma once

#include "Engine/Core/Transform/RigidTransform.h"
#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Math/TDecomposedTransform.h"
#include "Engine/Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <array>
#include <vector>

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
	static StaticRigidTransform makeForward(
		const math::TDecomposedTransform<U>& transform,
		bool ensureScaleFree = true);

	template<typename U>
	static StaticRigidTransform makeInverse(
		const math::TDecomposedTransform<U>& transform,
		bool ensureScaleFree = true);

	template<typename U>
	static StaticRigidTransform makeParentedForward(
		TSpanView<math::TDecomposedTransform<U>> fromRootToLocal,
		bool ensureScaleFree = true,
		bool allowDynamicAllocation = false);

	template<typename U>
	static StaticRigidTransform makeParentedInverse(
		TSpanView<math::TDecomposedTransform<U>> fromRootToLocal,
		bool ensureScaleFree = true,
		bool allowDynamicAllocation = false);

	template<typename U>
	static inline void getScaleFreeTransforms(
		TSpanView<math::TDecomposedTransform<U>> transforms,
		TSpan<math::TDecomposedTransform<U>> out_scaleFreeTransforms);

public:
	/*! @brief Creates an identity transform.
	*/
	StaticRigidTransform();

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
	explicit StaticRigidTransform(const StaticAffineTransform& transform);

	StaticAffineTransform m_staticTransform;
};

template<typename U>
inline auto StaticRigidTransform::makeForward(
	const math::TDecomposedTransform<U>& transform,
	const bool ensureScaleFree)
	-> StaticRigidTransform
{
	if(ensureScaleFree)
	{
		std::array<math::TDecomposedTransform<U>, 1> scaledFreeTransform;
		getScaleFreeTransforms<U>({&transform, 1}, scaledFreeTransform);

		return StaticRigidTransform(StaticAffineTransform::makeForward(scaledFreeTransform[0]));
	}
	else
	{
		return StaticRigidTransform(StaticAffineTransform::makeForward(transform));
	}
}

template<typename U>
inline auto StaticRigidTransform::makeInverse(
	const math::TDecomposedTransform<U>& transform,
	const bool ensureScaleFree)
	-> StaticRigidTransform
{
	if(ensureScaleFree)
	{
		std::array<math::TDecomposedTransform<U>, 1> scaledFreeTransform;
		getScaleFreeTransforms<U>({&transform, 1}, scaledFreeTransform);

		return StaticRigidTransform(StaticAffineTransform::makeInverse(scaledFreeTransform[0]));
	}
	else
	{
		return StaticRigidTransform(StaticAffineTransform::makeInverse(transform));
	}
}

template<typename U>
inline auto StaticRigidTransform::makeParentedForward(
	TSpanView<math::TDecomposedTransform<U>> fromRootToLocal,
	const bool ensureScaleFree,
	const bool allowDynamicAllocation)
	-> StaticRigidTransform
{
	if(ensureScaleFree)
	{
		if(allowDynamicAllocation)
		{
			std::vector<math::TDecomposedTransform<U>> scaleFreeTransforms(fromRootToLocal.size());
			getScaleFreeTransforms<U>(fromRootToLocal, scaleFreeTransforms);

			return StaticRigidTransform(StaticAffineTransform::makeParentedForward(scaleFreeTransforms));
		}
		else
		{
			std::array<math::TDecomposedTransform<U>, 4> scaleFreeTransforms;
			getScaleFreeTransforms<U>(fromRootToLocal, scaleFreeTransforms);

			return StaticRigidTransform(StaticAffineTransform::makeParentedForward(scaleFreeTransforms));
		}
	}
	else
	{
		return StaticRigidTransform(StaticAffineTransform::makeParentedForward(fromRootToLocal));
	}
}

template<typename U>
inline auto StaticRigidTransform::makeParentedInverse(
	TSpanView<math::TDecomposedTransform<U>> fromRootToLocal,
	const bool ensureScaleFree,
	const bool allowDynamicAllocation)
	-> StaticRigidTransform
{
	if(ensureScaleFree)
	{
		if(allowDynamicAllocation)
		{
			std::vector<math::TDecomposedTransform<U>> scaleFreeTransforms(fromRootToLocal.size());
			getScaleFreeTransforms<U>(fromRootToLocal, scaleFreeTransforms);

			return StaticRigidTransform(StaticAffineTransform::makeParentedInverse(scaleFreeTransforms));
		}
		else
		{
			std::array<math::TDecomposedTransform<U>, 4> scaleFreeTransforms;
			getScaleFreeTransforms<U>(fromRootToLocal, scaleFreeTransforms);

			return StaticRigidTransform(StaticAffineTransform::makeParentedInverse(scaleFreeTransforms));
		}
	}
	else
	{
		return StaticRigidTransform(StaticAffineTransform::makeParentedInverse(fromRootToLocal));
	}
}

template<typename U>
inline void StaticRigidTransform::getScaleFreeTransforms(
	TSpanView<math::TDecomposedTransform<U>> transforms,
	TSpan<math::TDecomposedTransform<U>> out_scaleFreeTransforms)
{
	PH_ASSERT_GE(out_scaleFreeTransforms.size(), transforms.size());

	for(std::size_t ti = 0; ti < transforms.size(); ++ti)
	{
		// FIXME: better scale tolerance value, not hardcoded like this
		if(!transforms[ti].hasScaleEffect(0.000001_r))
		{
			out_scaleFreeTransforms[ti] = transforms[ti];
		}
		else
		{
			PH_LOG(StaticRigidTransform, Warning,
				"scale effect detected, which is {}, ignoring", transforms[ti].getScale());

			out_scaleFreeTransforms[ti] = math::TDecomposedTransform<U>(transforms[ti]).setScale(1);
		}
	}
}

inline void StaticRigidTransform::doTransformVector(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	// Explicitly quality with `StaticAffineTransform` otherwise MSVC will still call through vtable;
	// same goes for the following inlined methods.
	m_staticTransform.StaticAffineTransform::doTransformVector(vector, time, out_vector);
}

inline void StaticRigidTransform::doTransformOrientation(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	m_staticTransform.StaticAffineTransform::doTransformOrientation(orientation, time, out_orientation);
}

inline void StaticRigidTransform::doTransformPoint(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	m_staticTransform.StaticAffineTransform::doTransformPoint(point, time, out_point);
}

inline void StaticRigidTransform::doTransformRay(
	const Ray& ray,
	Ray* const out_ray) const
{
	m_staticTransform.StaticAffineTransform::doTransformRay(ray, out_ray);
}

inline void StaticRigidTransform::doTransformHitInfo(
	const HitInfo& info,
	const Time&    time,
	HitInfo* const out_info) const
{
	m_staticTransform.StaticAffineTransform::doTransformHitInfo(info, time, out_info);
}

inline void StaticRigidTransform::doTransformLineSegment(
	const math::TLineSegment<real>& segment,
	const Time&                     time,
	math::TLineSegment<real>* const out_segment) const
{
	m_staticTransform.StaticAffineTransform::doTransformLineSegment(segment, time, out_segment);
}

}// end namespace ph::math
