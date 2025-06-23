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
		const math::TLineSegment<real>& segment,
		const Time&                     time,
		math::TLineSegment<real>*       out_segment) const override;

	math::Vector3R m_translationT0;
	math::Vector3R m_translationT1;
};

}// end namespace ph
