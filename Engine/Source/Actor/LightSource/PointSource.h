#pragma once

#include "Actor/LightSource/AreaSource.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph
{

class PointSource : public AreaSource
{
public:
	PointSource();
	PointSource(const math::Vector3R& linearSrgbColor, real numWatts);
	PointSource(const math::Spectrum& color, real numWatts);

	std::shared_ptr<Geometry> genAreas(ActorCookingContext& ctx) const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<PointSource>)
	{
		ClassType clazz("point");
		clazz.docName("Point Light Source");
		clazz.description(
			"Power emitting source from a small but not infinitesimal region. Resembling "
			"a small light bulb.");
		clazz.baseOn<AreaSource>();

		return clazz;
	}
};

}// end namespace ph
