#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Geometry/TAABB3D.h"

#include <Common/primitive_type.h>

#include <memory>

namespace ph
{

class Time;
class Ray;
class HitInfo;
class HitDetail;
class SurfaceHit;

}// end namespace ph

namespace ph
{

// TODO: templatize
// TODO: use line segment, not ray
class Transform
{
public:
	virtual ~Transform() = default;

	/*!
	@return Inversed transform. Null if not available.
	*/
	virtual std::unique_ptr<Transform> genInversed() const;

	/*! @brief Conservative bound swept by transforming `aabb` from `startTime` to `endTime`.
	*/
	void calcSweepAABB(
		const math::AABB3D& aabb,
		const Time&         startTime,
		const Time&         endTime,
		math::AABB3D*       out_aabb) const;

	/*! @brief Conservative bound swept by transforming `aabb` from `startTime` to `endTime`.
	@param numSamples Approximation hint for sampled implementations. May be ignored.
	@param paddingFactor Relative expansion factor based on swept AABB extents. `1` means no padding; values above `1` expand the bound.
	*/
	void calcSweepAABB(
		const math::AABB3D& aabb,
		const Time&         startTime,
		const Time&         endTime,
		uint32              numSamples,
		real                paddingFactor,
		math::AABB3D*       out_aabb) const;

	/*! @brief Whether transforming from `startTime` to `endTime` can produce time-varying results.
	*/
	virtual bool hasMotion(const Time& startTime, const Time& endTime) const;

	/*!
	Treating a Vector3R as either
	
	1. Vectors      (V)
	2. Orientations (O) (such as normals and tangents)
	3. Points       (P) 
	
	and calculate the transformed result.
	*/
	///@{
	void transformV(
		const math::Vector3R& vector, 
		math::Vector3R*       out_vector) const;

	void transformO(
		const math::Vector3R& orientation,
		math::Vector3R*       out_orientation) const;

	void transformP(
		const math::Vector3R& point,
		math::Vector3R*       out_point) const;

	void transformV(
		const math::Vector3R& vector,
		const Time&           time,
		math::Vector3R*       out_vector) const;

	void transformO(
		const math::Vector3R& orientation,
		const Time&           time,
		math::Vector3R*       out_orientation) const;

	void transformP(
		const math::Vector3R& point, 
		const Time&           time,
		math::Vector3R*       out_point) const;
	///@}

	/*! 
	Notice that transforming a ray neither will change its parametric distance (t) nor renormalizing
	its direction vector even if the transform contains scale factor; because if users respect
	the ray segment's definition:

	```
		Ray Segment = [rayOrigin + rayMinT * rayDirection, 
		               rayOrigin + rayMaxT * rayDirection]
	```

	this operation will always yield a correctly transformed result while saving an expensive `sqrt()` call.
	*/
	void transform(
		const Ray& ray,
		Ray*       out_ray) const;

	void transform(
		const HitInfo& info,
		HitInfo*       out_info) const;

	/*!
	Will only affect world-space hit info.
	*/
	void transform(
		const HitDetail& detail,
		HitDetail*       out_detail) const;

	/*!
	Will only affect world-space hit info. Full hit detail will be automatically calculated if
	the original hit detail is full.
	*/
	void transform(
		const SurfaceHit& surfaceHit,
		SurfaceHit*       out_surfaceHit) const;

	void transform(
		const math::AABB3D& aabb,
		math::AABB3D*       out_aabb) const;

	void transform(
		const HitInfo& info,
		const Time&    time,
		HitInfo*       out_info) const;

	void transform(
		const HitDetail& detail,
		const Time&      time,
		HitDetail*       out_detail) const;

	void transform(
		const math::AABB3D& aabb, 
		const Time&         time,
		math::AABB3D*       out_aabb) const;

private:
	/*! @brief Build an unpadded swept bound.
	Implementations may ignore `numSamples`.
	*/
	virtual void doCalcSweepAABB(
		const math::AABB3D& aabb,
		const Time&         startTime,
		const Time&         endTime,
		uint32              numSamples,
		math::AABB3D*       out_aabb) const;

	/*! @brief Optional fast paths for composite transform operations.
	Derived implementations must match the default implementation's behavior.
	*/
	///@{
	virtual void doTransformRay(
		const Ray& ray,
		Ray*       out_ray) const;

	virtual void doTransformHitInfo(
		const HitInfo& info,
		const Time&    time,
		HitInfo*       out_info) const;
	///@}

	/*! @brief Treating a `Vector3R` as either a vector, orientation, or point and calculate the transformed result.
	*/
	///@{
	virtual void doTransformVector(
		const math::Vector3R& vector, 
		const Time&           time,
		math::Vector3R*       out_vector) const = 0;

	virtual void doTransformOrientation(
		const math::Vector3R& orientation, 
		const Time&           time,
		math::Vector3R*       out_orientation) const = 0;

	virtual void doTransformPoint(
		const math::Vector3R& point, 
		const Time&           time,
		math::Vector3R*       out_point) const = 0;
	///@}

	/*! @brief Transform the specified line segment.
	Also note that line direction is not necessary normalized.
	*/
	virtual void doTransformLineSegment(
		const math::TLineSegment<real>& segment,
		const Time&                     time,
		math::TLineSegment<real>*       out_segment) const = 0;
};

inline bool Transform::hasMotion(const Time& startTime, const Time& endTime) const
{
	return false;
}

inline void Transform::calcSweepAABB(
	const math::AABB3D& aabb,
	const Time&         startTime,
	const Time&         endTime,
	math::AABB3D* const out_aabb) const
{
	calcSweepAABB(aabb, startTime, endTime, 128, 1.0_r, out_aabb);
}

inline void Transform::transform(
	const Ray& ray,
	Ray* const out_ray) const
{
	doTransformRay(ray, out_ray);
}

inline void Transform::transform(
	const HitInfo& info,
	const Time&    time,
	HitInfo* const out_info) const
{
	doTransformHitInfo(info, time, out_info);
}

}// end namespace ph
