#pragma once

#include "Math/Transform/Transform.h"

namespace ph::math
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
		const Vector3R& vector, 
		const Time&     time, 
		Vector3R*       out_vector) const override = 0;

	void transformOrientation(
		const Vector3R& orientation, 
		const Time&     time,
		Vector3R*       out_orientation) const override = 0;

	void transformPoint(
		const Vector3R& point, 
		const Time&     time, 
		Vector3R*       out_point) const override = 0;

	void transformLineSegment(
		const Vector3R& lineStartPos, 
		const Vector3R& lineDir, 
		real            lineMinT, 
		real            lineMaxT, 
		const Time&     time, 
		Vector3R*       out_lineStartPos, 
		Vector3R*       out_lineDir,
		real*           out_lineMinT, 
		real*           out_lineMaxT) const override = 0;
};

}// end namespace ph::math
