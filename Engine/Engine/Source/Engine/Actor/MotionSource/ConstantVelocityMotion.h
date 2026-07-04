#pragma once

#include "Engine/Actor/MotionSource/MotionSource.h"
#include "Engine/Math/TVector3.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class ConstantVelocityMotion : public MotionSource
{
public:
	ConstantVelocityMotion();

	void storeCooked(
		const CookingContext& ctx,
		CookedMotion& out_motion) const override;

private:
	math::Vector3R m_velocity;

public:
	PH_DEFINE_SDL_CLASS(ConstantVelocityMotion, clazz)
	{
		clazz.typeName("constant-velocity");
		clazz.docName("Constant Velocity Motion Source");
		clazz.description("Motion source with a constant linear velocity.");
		clazz.baseOn<MotionSource>();

		TSdlVector3<OwnerType> velocity("velocity", &OwnerType::m_velocity);
		velocity.description("Linear velocity in world units per second.");
		clazz.addField(velocity);
	}
};

}// end namespace ph
