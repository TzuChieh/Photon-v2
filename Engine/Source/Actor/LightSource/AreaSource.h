#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Core/Quantity/Spectrum.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

#include <memory>

namespace ph
{

class Image;

class AreaSource : public LightSource
{
public:
	AreaSource();
	AreaSource(const math::Vector3R& linearSrgbColor, real numWatts);
	AreaSource(const Spectrum& color, real numWatts);

	virtual std::shared_ptr<Geometry> genAreas(ActorCookingContext& ctx) const = 0;

	std::unique_ptr<Emitter> genEmitter(
		ActorCookingContext& ctx, EmitterBuildingMaterial&& data) const override;

	std::shared_ptr<Geometry> genGeometry(ActorCookingContext& ctx) const final override;

private:
	Spectrum m_color;
	real     m_numWatts;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<AreaSource>)
	{
		ClassType clazz("area");
		clazz.docName("Area Light Source");
		clazz.description(
			"This type of light source has a finite area. Energy is allowed to"
			"emit as long as the emitting source is within the area.");
		clazz.baseOn<LightSource>();

		TSdlSpectrum<OwnerType> color("color", EQuantity::EMR, &OwnerType::m_color);
		color.description("The color of this light source.");
		color.defaultTo(Spectrum().setLinearSrgb({1, 1, 1}, EQuantity::EMR));
		clazz.addField(color);

		TSdlReal<OwnerType> numWatts("watts", &OwnerType::m_numWatts);
		numWatts.description("Energy emitted by this light source, in watts.");
		numWatts.defaultTo(100);
		clazz.addField(numWatts);

		return clazz;
	}
};

}// end namespace ph
