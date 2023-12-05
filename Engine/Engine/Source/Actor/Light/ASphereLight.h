#pragma once

#include "Actor/Light/AAreaLight.h"
#include "SDL/sdl_interface.h"

#include <Common/primitive_type.h>

namespace ph
{

class ASphereLight : public AAreaLight
{
public:
	std::shared_ptr<Geometry> getArea(const CookingContext& ctx) const override;

	void setRadius(real radius);

private:
	real m_radius;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ASphereLight>)
	{
		ClassType clazz("sphere-light");
		clazz.docName("Spherical Light Actor");
		clazz.description(
			"This type of light emits energy from a spherical shape.");
		clazz.baseOn<AAreaLight>();

		TSdlReal<OwnerType> radius("radius", &OwnerType::m_radius);
		radius.description("The radius of the sphere.");
		radius.defaultTo(1);
		clazz.addField(radius);

		return clazz;
	}
};

}// end namespace ph
