#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class MatteOpaque : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

	/*!
	@param albedo Albedo in linear-sRGB.
	*/
	void setAlbedo(const math::Vector3R& albedo);

	/*! @brief Same as setAlbedo(const math::Vector3R&).
	*/
	void setAlbedo(const real r, const real g, const real b);

	void setAlbedoMap(std::shared_ptr<Image> albedoMap);
	void setSigma(real sigma);
	void setSigmaMap(std::shared_ptr<Image> sigmaMap);

private:
	math::Spectrum m_albedo;
	std::shared_ptr<Image> m_albedoMap;
	real m_sigma;
	std::shared_ptr<Image> m_sigmaMap;

public:
	PH_DEFINE_SDL_CLASS(MatteOpaque, clazz)
	{
		clazz.typeName("matte-opaque");
		clazz.docName("Matte Opaque Material");
		clazz.description("A material model for surfaces with matte look, such as chalk and moon. For paired value/map inputs, map inputs have higher precedence.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlSpectrum<OwnerType> albedo("albedo", math::EColorUsage::ECF, &OwnerType::m_albedo);
		albedo.description("Constant albedo in linear-sRGB.");
		albedo.defaultTo(math::Spectrum(0.5_r));
		albedo.optional();
		clazz.addField(albedo);

		TSdlReference<Image, OwnerType> albedoMap("albedo-map", &OwnerType::m_albedoMap);
		albedoMap.description("Texture-mapped albedo.");
		albedoMap.optional();
		clazz.addField(albedoMap);

		TSdlReal<OwnerType> sigma("sigma", &OwnerType::m_sigma);
		sigma.description(
			"Constant Oren-Nayar sigma, the standard deviation of surface orientation. "
			"The value is used directly (unit: radians, normally in [0, 1], but you can also use higher values). "
			"If the sigma is 0, it is equivalent to Lambertian diffuse as all facets are on the "
			"same macrosurface plane.");
		sigma.defaultTo(0.0_r);
		sigma.optional();
		clazz.addField(sigma);

		TSdlReference<Image, OwnerType> sigmaMap("sigma-map", &OwnerType::m_sigmaMap);
		sigmaMap.description(
			"Texture-mapped Oren-Nayar sigma. "
			"The sampled value is used directly (unit: radians, in [0, 1]).");
		sigmaMap.optional();
		clazz.addField(sigmaMap);
	}
};

}// end namespace ph
