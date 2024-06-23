#pragma once

#include "Actor/Light/AAreaLight.h"
#include "SDL/sdl_interface.h"

namespace ph
{

class APointLight : public AAreaLight
{
public:
	std::shared_ptr<Geometry> getArea(const CookingContext& ctx) const override;

protected:
	EmitterFeatureSet getEmitterFeatureSet() const override;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<APointLight>)
	{
		ClassType clazz("point-light");
		clazz.docName("Point Light Actor");
		clazz.description(
			"Power emitting source from a small but not infinitesimal region. Resembling "
			"a small light bulb.");
		clazz.baseOn<AAreaLight>();

		return clazz;
	}
};

}// end namespace ph
