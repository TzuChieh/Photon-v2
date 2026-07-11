#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/SDLExtension/TSdlSpectrum.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

enum class ESurfaceMaterialMixMode
{
	Lerp = 0
};

PH_DEFINE_SDL_ENUM(ESurfaceMaterialMixMode, e)
{
	e.name("surface-material-mix-mode");
	e.description("Specify how surface materials are mixed.");

	e.addEntry(EnumType::Lerp, "lerp");
}

class BinaryMixedSurfaceMaterial : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

	void setMode(ESurfaceMaterialMixMode mode);

	void setMaterials(
		std::shared_ptr<SurfaceMaterial> material0,
		std::shared_ptr<SurfaceMaterial> material1);

	void setFactor(real factor);
	void setFactor(const math::Spectrum& factor);
	void setFactor(std::shared_ptr<Image> factor);
	void setFactorMap(std::shared_ptr<Image> factorMap);

private:
	ESurfaceMaterialMixMode m_mode;
	std::shared_ptr<SurfaceMaterial> m_material0;
	std::shared_ptr<SurfaceMaterial> m_material1;
	math::Spectrum m_factor;
	std::shared_ptr<Image> m_factorMap;

public:
	PH_DEFINE_SDL_CLASS(BinaryMixedSurfaceMaterial, clazz)
	{
		clazz.typeName("binary-mixed-surface");
		clazz.docName("Binary Mixed Surface");
		clazz.description(
			"Mixing two surface materials in various ways. For paired value/map inputs, "
			"map inputs have higher precedence.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlEnumField<OwnerType, ESurfaceMaterialMixMode> mode("mode", &OwnerType::m_mode);
		mode.description("Specify how two materials are mixed.");
		mode.defaultTo(ESurfaceMaterialMixMode::Lerp);
		mode.required();
		clazz.addField(mode);

		TSdlReference<SurfaceMaterial, OwnerType> material0("material-0", &OwnerType::m_material0);
		material0.description("The first material that participates the mixing process.");
		material0.required();
		clazz.addField(material0);

		TSdlReference<SurfaceMaterial, OwnerType> material1("material-1", &OwnerType::m_material1);
		material1.description("The second material that participates the mixing process.");
		material1.required();
		clazz.addField(material1);

		TSdlSpectrum<OwnerType> factor("factor", math::EColorUsage::ECF, &OwnerType::m_factor);
		factor.description(
			"Factor that controls the contribution from each material. Basically, the final material "
			"would be \"material-0 * factor + material-1 * (1 - factor)\".");
		factor.defaultTo(math::Spectrum(0.5_r));
		factor.optional();// defaults to an even mix when not provided
		clazz.addField(factor);

		TSdlReference<Image, OwnerType> factorMap("factor-map", &OwnerType::m_factorMap);
		factorMap.description("Texture-mapped factor that controls the contribution from each material.");
		factorMap.optional();
		clazz.addField(factorMap);
	}
};

}// end namespace ph
