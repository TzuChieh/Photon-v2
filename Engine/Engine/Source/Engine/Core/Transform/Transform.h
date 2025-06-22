#pragma once

#include "Engine/Math/math_fwd.h"
#include "Engine/Math/Geometry/TAABB3D.h"

#include <memory>

namespace ph
{
	class Time;
	class Ray;
	class HitInfo;
}

namespace ph
{

// TODO: templatize
// TODO: use line segment, not ray
class Transform
{
public:
	virtual ~Transform() = default;

	// DEPRECATED
	virtual std::unique_ptr<Transform> genInversed() const;

	// Treating a Vector3R as either
	//
	// 1) vectors      (V)
	// 2) orientations (O) (such as normals and tangents)
	// 3) points       (P) 
	//
	// and calculate the transformed result.

	void transformV(const math::Vector3R& vector, 
	                math::Vector3R* out_vector) const;
	void transformO(const math::Vector3R& orientation,
	                math::Vector3R* out_orientation) const;
	void transformP(const math::Vector3R& point,
	                math::Vector3R* out_point) const;

	void transformV(const math::Vector3R& vector, const Time& time,
	                math::Vector3R* out_vector) const;
	void transformO(const math::Vector3R& orientation, const Time& time,
	                math::Vector3R* out_orientation) const;
	void transformP(const math::Vector3R& point, const Time& time,
	                math::Vector3R* out_point) const;

	// Notice that transforming a ray neither will change its parametric 
	// distance (t) nor renormalizing its direction vector even if the transform 
	// contains scale factor; because if users respect the ray segment's 
	// definition:
	// 
	//     Ray Segment = [rayOrigin + rayMinT * rayDirection, 
	//                    rayOrigin + rayMaxT * rayDirection]
	// 
	// this operation will always yield a correctly transformed result while 
	// saving an expensive sqrt() call.
	//
	void transform(const Ray& ray, 
	               Ray* out_ray) const;

	void transform(const HitInfo& info,
	               HitInfo* out_info) const;
	void transform(const math::AABB3D& aabb,
	               math::AABB3D* out_aabb) const;

	void transform(const HitInfo& info, const Time& time,
	               HitInfo* out_info) const;
	void transform(const math::AABB3D& aabb, const Time& time,
	               math::AABB3D* out_aabb) const;

private:

	// Treating a Vector3R as either a vector, orientation, or point and 
	// calculate the transformed result.

	virtual void transformVector(const math::Vector3R& vector, const Time& time,
	                             math::Vector3R* out_vector) const = 0;

	virtual void transformOrientation(const math::Vector3R& orientation, const Time& time,
	                                  math::Vector3R* out_orientation) const = 0;

	virtual void transformPoint(const math::Vector3R& point, const Time& time,
	                            math::Vector3R* out_point) const = 0;

	// Transform the specified line segment. A line segment's definition is 
	// as follows:
	// 
	//     Line Segment = [lineStartPos + lineMinT * lineDir,
	//                     lineStartPos + lineMaxT * lineDir]
	//
	// Also note that lineDir is not necessary to be normalized.
	//
	virtual void transformLineSegment(const math::Vector3R& lineStartPos, const math::Vector3R& lineDir,
	                                  real lineMinT, real lineMaxT, const Time& time, 
	                                  math::Vector3R* out_lineStartPos, math::Vector3R* out_lineDir,
	                                  real* out_lineMinT, real* out_lineMaxT) const = 0;
};

}// end namespace ph
