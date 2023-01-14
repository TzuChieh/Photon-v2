#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class SphereSource : public AreaSource
{
public:
	SphereSource();
	SphereSource(real radius, const math::Vector3R& linearSrgbColor, real numWatts);
	SphereSource(real radius, const math::Spectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(CookingContext& ctx) const override;

	void setRadius(real radius);

private:
	real m_radius;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SphereSource>)
	{
		ClassType clazz("sphere");
		clazz.docName("Spherical Light Source");
		clazz.description(
			"This type of light emits energy from a spherical shape.");
		clazz.baseOn<AreaSource>();

		TSdlReal<OwnerType> radius("radius", &OwnerType::m_radius);
		radius.description("The radius of the sphere.");
		radius.defaultTo(1);
		clazz.addField(radius);

		return clazz;
	}
};

}// end namespace ph
