#pragma once

#include "Actor/MotionSource/MotionSource.h"
#include "Math/TVector3.h"

namespace ph
{

class ConstantVelocityMotion : public MotionSource
{
public:
	// TODO: remove
	explicit ConstantVelocityMotion(const math::Vector3R& velocity);

	void storeCooked(
		CookedMotion& out_motion,
		const CookingContext& ctx,
		const MotionCookConfig& config) const override;

	std::unique_ptr<math::Transform> genLocalToWorld(
		const Time& start,
		const Time& end) const override;

private:
	math::Vector3R m_velocity;

public:
	// TODO: SDL interface
};

}// end namespace ph
