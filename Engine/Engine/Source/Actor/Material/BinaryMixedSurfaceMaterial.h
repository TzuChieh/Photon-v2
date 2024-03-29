#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Actor/Image/Image.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

enum class ESurfaceMaterialMixMode
{
	Lerp = 0
};

PH_DEFINE_SDL_ENUM(TSdlGeneralEnum<ESurfaceMaterialMixMode>)
{
	SdlEnumType sdlEnum("surface-material-mix-mode");
	sdlEnum.description("Specify how surface materials are mixed.");

	sdlEnum.addEntry(EnumType::Lerp, "lerp");

	return sdlEnum;
}

class BinaryMixedSurfaceMaterial : public SurfaceMaterial
{
public:
	void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const override;

	void setMode(ESurfaceMaterialMixMode mode);

	void setMaterials(
		std::shared_ptr<SurfaceMaterial> material0,
		std::shared_ptr<SurfaceMaterial> material1);

	void setFactor(real factor);
	void setFactor(std::shared_ptr<Image> factor);

private:
	ESurfaceMaterialMixMode m_mode;
	std::shared_ptr<SurfaceMaterial> m_material0;
	std::shared_ptr<SurfaceMaterial> m_material1;
	std::shared_ptr<Image> m_factor;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<BinaryMixedSurfaceMaterial>)
	{
		ClassType clazz("binary-mixed-surface");
		clazz.docName("Binary Mixed Surface");
		clazz.description("Mixing two surface materials in various ways.");
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

		TSdlReference<Image, OwnerType> factor("factor", &OwnerType::m_factor);
		factor.description(
			"Factor that controls the contribution from each material. Basically, the final material "
			"would be \"material-0 * factor + material-1 * (1 - factor)\".");
		factor.optional();// some operation might not need a factor; check factor at cook time
		clazz.addField(factor);

		return clazz;
	}
};

}// end namespace ph
