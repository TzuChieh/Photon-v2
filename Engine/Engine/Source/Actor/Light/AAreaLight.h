#pragma once

#include "Actor/Light/AGeometricLight.h"
#include "Math/Color/Spectrum.h"
#include "SDL/sdl_interface.h"
#include "Actor/SDLExtension/TSdlSpectrum.h"

#include <Common/primitive_type.h>

namespace ph
{

class AAreaLight : public AGeometricLight
{
public:
	virtual std::shared_ptr<Geometry> getArea(const CookingContext& ctx) const = 0;

	std::shared_ptr<Geometry> getGeometry(const CookingContext& ctx) const override;
	std::shared_ptr<Material> getMaterial(const CookingContext& ctx) const override;

	const Emitter* buildEmitter(
		const CookingContext& ctx,
		TSpanView<const Primitive*> lightPrimitives) const override;

private:
	math::Spectrum m_color;
	real m_numWatts;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AAreaLight>)
	{
		ClassType clazz("area-light");
		clazz.docName("Area Light Actor");
		clazz.description(
			"This type of light source has a finite area, with various simplifications on the "
			"characteristics of the emission profile. Energy is allowed to emit diffusively and "
			"uniformally within the area. By default, the energy emitting surface absorbs all "
			"incoming energy (which is effectively a pure absorber).");
		clazz.baseOn<AGeometricLight>();

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
