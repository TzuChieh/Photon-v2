#pragma once

#include "Actor/Image/Image.h"
#include "Common/primitive_type.h"
#include "Actor/SDLExtension/sdl_color_enums.h"
#include "SDL/sdl_interface.h"

#include <optional>

namespace ph
{

class BlackBodyRadiationImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	/*! @brief Get the black-body radiation as defined by the input parameters.
	*/
	math::Spectrum getRadiation() const;

private:
	real m_temperatureK;
	bool m_isSpectralRadiance;
	std::optional<real> m_energy;
	math::EColorSpace m_numericColorSpace;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<BlackBodyRadiationImage>)
	{
		ClassType clazz("black-body");
		clazz.docName("Black-body Radiation Image");
		clazz.description(
			"An image outputs the value of black-body radiation.");
		clazz.baseOn<Image>();

		TSdlReal<OwnerType> temperatureK("temperature-k", &OwnerType::m_temperatureK);
		temperatureK.description(
			"Temperature (in Kelvin) that the black-body radiates on.");
		temperatureK.defaultTo(3000.0_r);
		temperatureK.required();
		clazz.addField(temperatureK);

		TSdlBool<OwnerType> isSpectralRadiance("is-spectral-radiance", &OwnerType::m_isSpectralRadiance);
		isSpectralRadiance.description(
			"false (default): The energy unit is in radiance; true: The energy unit is in spectral "
			"radiance. If \"energy\" value is specified, this option will have no effect (the user "
			"is then responsible for specifying \"energy\" value in their desired unit).");
		isSpectralRadiance.defaultTo(false);
		isSpectralRadiance.optional();
		clazz.addField(isSpectralRadiance);

		TSdlOptionalReal<OwnerType> energy("energy", &OwnerType::m_energy);
		energy.description(
			"If specified, the resulting radiation will be adjusted (scaled) to the target "
			"energy level; otherwise, the true energy level at the temperature will be used.");
		clazz.addField(energy);

		TSdlEnumField<OwnerType, math::EColorSpace> numericColorSpace(&OwnerType::m_numericColorSpace);
		numericColorSpace.description(
			"The tristimulus color space to use when using the image as a numeric texture. "
			"The default is to use the current tristimulus space, and linear-sRGB when the engine "
			"is in spectral mode.");
		numericColorSpace.defaultTo(math::EColorSpace::Unspecified);
		numericColorSpace.optional();
		clazz.addField(numericColorSpace);

		return clazz;
	}
};

}// end namespace ph
