#pragma once

#include "Actor/LightSource/LightSource.h"
#include "Math/Color/Spectrum.h"
#include "Actor/Geometry/Geometry.h"
#include "SDL/sdl_interface.h"
#include "Actor/SDLExtension/TSdlSpectrum.h"

#include <memory>

namespace ph
{

class Image;

class AreaSource : public LightSource
{
public:
	AreaSource();
	AreaSource(const math::Vector3R& linearSrgbColor, real numWatts);
	AreaSource(const math::Spectrum& color, real numWatts);

	virtual std::shared_ptr<Geometry> genAreas(CookingContext& ctx) const = 0;

	std::unique_ptr<Emitter> genEmitter(
		CookingContext& ctx, EmitterBuildingMaterial&& data) const override;

	std::shared_ptr<Geometry> genGeometry(CookingContext& ctx) const final override;

private:
	math::Spectrum m_color;
	real           m_numWatts;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AreaSource>)
	{
		ClassType clazz("area");
		clazz.docName("Area Light Source");
		clazz.description(
			"This type of light source has a finite area. Energy is allowed to"
			"emit as long as the emitting source is within the area.");
		clazz.baseOn<LightSource>();

		TSdlSpectrum<OwnerType> color("color", math::EColorUsage::EMR, &OwnerType::m_color);
		color.description("The color of this light source.");
		color.defaultTo(math::Spectrum().setLinearSRGB({1, 1, 1}, math::EColorUsage::EMR));
		clazz.addField(color);

		TSdlReal<OwnerType> numWatts("watts", &OwnerType::m_numWatts);
		numWatts.description("Energy emitted by this light source, in watts.");
		numWatts.defaultTo(100);
		clazz.addField(numWatts);

		return clazz;
	}
};

}// end namespace ph
