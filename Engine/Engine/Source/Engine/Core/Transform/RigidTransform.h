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
	void doTransformVector(
		const math::Vector3R& vector,
		const Time&           time, 
		math::Vector3R*       out_vector) const override = 0;

	void doTransformOrientation(
		const math::Vector3R& orientation,
		const Time&           time,
		math::Vector3R*       out_orientation) const override = 0;

	void doTransformPoint(
		const math::Vector3R& point,
		const Time&           time, 
		math::Vector3R*       out_point) const override = 0;

	void doTransformLineSegment(
		const math::TLineSegment<real>& segment,
		const Time&                     time, 
		math::TLineSegment<real>*       out_segment) const override = 0;
};

}// end namespace ph
