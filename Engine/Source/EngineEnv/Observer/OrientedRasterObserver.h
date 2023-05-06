#pragma once

#include "EngineEnv/Observer/Observer.h"
#include "SDL/sdl_interface.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Common/assertion.h"

#include <optional>

namespace ph
{

class OrientedRasterObserver : public Observer
{
public:
	inline OrientedRasterObserver() = default;

	void cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked) override = 0;

protected:
	math::TDecomposedTransform<float64> makeObserverPose() const;
	math::Vector3D makePosition() const;
	math::QuaternionD makeRotation() const;
	math::Vector3D makeDirection() const;

private:
	math::Vector3R m_position;
	math::Vector3R m_yawPitchRollDegrees;

	std::optional<math::Vector3R> m_direction;
	std::optional<math::Vector3R> m_upAxis;

	static math::QuaternionD makeRotationFromVectors(const math::Vector3R& direction, const math::Vector3R& upAxis);
	static math::QuaternionD makeRotationFromYawPitchRoll(real yawDegrees, real pitchDegrees, real rollDegrees);

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<OrientedRasterObserver>)
	{
		ClassType clazz("oriented-raster");
		clazz.docName("Oriented Raster Observer");
		clazz.description(
			"Observers that work by projecting incoming energy in certain ways. "
			"Projective observers face the -z axis (+y up) and reside on (0, 0, 0) by default.");
		clazz.baseOn<Observer>();

		TSdlVector3<OwnerType> position("position", &OwnerType::m_position);
		position.description("Position of the observer.");
		position.defaultTo({0, 0, 0});
		clazz.addField(position);

		TSdlVector3<OwnerType> yawPitchRollDegrees("yaw-pitch-row-degrees", &OwnerType::m_yawPitchRollDegrees);
		yawPitchRollDegrees.description(
			"Direction that this observer is looking at in yaw pitch form. "
			"yaw: Rotation around +y axis in [-180, 180]; "
			"pitch: Declination from the horizon in [-90, 90]; "
			"row: Rotation around +z axis in [-180, 180].");
		yawPitchRollDegrees.defaultTo({0, 0, 0});
		yawPitchRollDegrees.optional();
		clazz.addField(yawPitchRollDegrees);

		TSdlOptionalVector3<OwnerType> direction("direction", &OwnerType::m_direction);
		direction.description("Direction vector that this observer is looking at. No need to be normalized.");
		clazz.addField(direction);

		TSdlOptionalVector3<OwnerType> upAxis("up-axis", &OwnerType::m_upAxis);
		upAxis.description("The direction vector that this observer consider as upward. No need to be normalized.");
		clazz.addField(upAxis);

		return clazz;
	}
};

}// end namespace ph
