#pragma once

#include "Actor/Observer/Observer.h"
#include "DataIO/SDL/sdl_interface.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/Transform/TDecomposedTransform.h"

namespace ph
{

class ProjectiveObserver : public Observer
{
public:
	ProjectiveObserver();
	ProjectiveObserver(const ProjectiveObserver& other);

	virtual std::unique_ptr<Receiver> genReceiver(const CoreCookingContext& ctx) = 0;

	ProjectiveObserver& operator = (const ProjectiveObserver& rhs);

	friend void swap(ProjectiveObserver& first, ProjectiveObserver& second);

private:
	math::Vector3R m_position;
	math::Vector3R m_direction;
	math::Vector2S m_resolution;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<ProjectiveObserver>)
	{
		ClassType clazz("projective");
		clazz.description("Observers that work by projecting incoming energy in certain ways.");
		clazz.baseOn<Observer>();

		// TODO: optional fields to support yaw & pitch input?

		TSdlVector3<OwnerType> position("position", &OwnerType::m_position);
		position.description("Position of the observer.");
		position.defaultTo({0, 0, 0});
		clazz.addField(position);

		TSdlVector3<OwnerType> direction("direction", &OwnerType::m_direction);
		direction.description("Direction that this observer is looking at.");
		direction.defaultTo({0, 0, -1});
		clazz.addField(direction);

		TSdlVector2S<OwnerType> resolution("resolution", &OwnerType::m_resolution);
		resolution.description("Observer resolution in x & y dimensions.");
		resolution.defaultTo({960, 540});
		clazz.addField(resolution);

		return clazz;
	}
};

}// end namespace ph
