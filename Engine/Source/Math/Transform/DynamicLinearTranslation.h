#pragma once

#include "Math/Transform/RigidTransform.h"
#include "Math/TVector3.h"

namespace ph
{

class DynamicLinearTranslation : public RigidTransform
{
public:
	DynamicLinearTranslation(const Vector3R& translationT0, 
	                         const Vector3R& translationT1);
	virtual ~DynamicLinearTranslation() override;

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

	Vector3R m_translationT0;
	Vector3R m_translationT1;
};

}// end namespace ph