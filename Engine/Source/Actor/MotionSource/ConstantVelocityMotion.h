#pragma once

#include "Actor/MotionSource/MotionSource.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstantVelocityMotion : public MotionSource
{
public:
	explicit ConstantVelocityMotion(const math::Vector3R& velocity);

	std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start,
		const Time& end) const override;

private:
	math::Vector3R m_velocity;
};

}// end namespace ph
