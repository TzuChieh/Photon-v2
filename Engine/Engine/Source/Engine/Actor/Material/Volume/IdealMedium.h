#pragma once

#include "Engine/Actor/Material/VolumeMaterial.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/SDL/sdl_interface.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"

namespace ph
{

enum class EIdealMedium
{
	AbsorbtionOnly = 0,
};

PH_DEFINE_SDL_ENUM(EIdealMedium, e)
{
	e.name("ideal-medium");
	e.description("Type of the physical behavior of a constant and uniform medium.");

	e.addEntry(EnumType::AbsorbtionOnly, "absorption-only");
}

class IdealMedium : public VolumeMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

private:
	math::Spectrum m_absorptionCoeff;

public:
	PH_DEFINE_SDL_CLASS(IdealMedium, clazz)
	{
		clazz.typeName("ideal-medium");
		clazz.docName("Ideal Medium Material");
		clazz.description(
			"A material model for volumes with a uniform distribution of medium. Medium properties "
			"are constant throughout the volume.");
		clazz.baseOn<VolumeMaterial>();

		TSdlSpectrum<OwnerType> absorptionCoeff("absorption-coeff", math::EColorUsage::Raw, &OwnerType::m_absorptionCoeff);
		absorptionCoeff.description(
			"A scaling factor for reflected energy. Note that this property is only for "
			"artistic control and is not physically correct.");
		absorptionCoeff.defaultTo(math::Spectrum(0.5_r));
		absorptionCoeff.optional();
		clazz.addField(absorptionCoeff);
	}
};

}// end namespace ph
