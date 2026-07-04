#pragma once

#include "Engine/Core/Transform/RigidTransform.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Math/Geometry/TLineSegment.h"

namespace ph
{

class DynamicLinearTranslation : public RigidTransform
{
public:
	DynamicLinearTranslation(const math::Vector3R& translationT0,
	                         const math::Vector3R& translationT1);

	DynamicLinearTranslation makeInversed() const;

	std::unique_ptr<Transform> genInversed() const override;

	bool hasMotion(const Time& startTime, const Time& endTime) const override;

private:
	void doCalcSweepAABB(
		const math::AABB3D& aabb,
		const Time&         startTime,
		const Time&         endTime,
		uint32              numSamples,
		math::AABB3D*       out_aabb) const override;

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

	math::Vector3R m_translationT0;
	math::Vector3R m_translationT1;
};

}// end namespace ph
