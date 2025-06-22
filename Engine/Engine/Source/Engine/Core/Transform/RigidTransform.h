#pragma once

#include "Engine/Core/Transform/Transform.h"

namespace ph
{

/*
	An abstraction for transforms that enforce object rigidity during 
	the transformation process. Implementations should follow this 
	requirement strictly.
*/
class RigidTransform : public Transform
{
private:
	void transformVector(
		const math::Vector3R& vector,
		const Time&           time, 
		math::Vector3R*       out_vector) const override = 0;

	void transformOrientation(
		const math::Vector3R& orientation,
		const Time&           time,
		math::Vector3R*       out_orientation) const override = 0;

	void transformPoint(
		const math::Vector3R& point,
		const Time&           time, 
		math::Vector3R*       out_point) const override = 0;

	void transformLineSegment(
		const math::Vector3R& lineStartPos,
		const math::Vector3R& lineDir,
		real                  lineMinT, 
		real                  lineMaxT, 
		const Time&           time, 
		math::Vector3R*       out_lineStartPos,
		math::Vector3R*       out_lineDir,
		real*                 out_lineMinT, 
		real*                 out_lineMaxT) const override = 0;
};

}// end namespace ph
