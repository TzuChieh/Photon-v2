#pragma once

#include "Actor/LightSource/SphereSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class PointSource : public SphereSource
{
public:
	PointSource();
	PointSource(const math::Vector3R& linearSrgbColor, real numWatts);
	PointSource(const math::Spectrum& color, real numWatts);

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PointSource>)
	{
		ClassType clazz("point");
		clazz.docName("Point Light Source");
		clazz.description(
			"Power emitting source from a small but not infinitesimal region. Resembling "
			"a small light bulb.");
		clazz.baseOn<SphereSource>();

		return clazz;
	}
};

}// end namespace ph
