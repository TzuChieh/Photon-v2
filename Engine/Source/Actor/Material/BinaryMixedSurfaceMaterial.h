#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "SDL/sdl_interface.h"
#include "Actor/SDLExtension/TSdlUnifiedColorImage.h"

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

class BinaryMixedSurfaceMaterial final : public SurfaceMaterial
{
public:
	BinaryMixedSurfaceMaterial();

	void genSurface(CookingContext& ctx, SurfaceBehavior& behavior) const override;

	void setMode(ESurfaceMaterialMixMode mode);

	void setMaterials(
		std::shared_ptr<SurfaceMaterial> material0,
		std::shared_ptr<SurfaceMaterial> material1);

	void setFactor(real factor);
	void setFactor(std::shared_ptr<Image> factor);

protected:
	UnifiedColorImage* getFactor();

private:
	ESurfaceMaterialMixMode            m_mode;
	std::shared_ptr<SurfaceMaterial>   m_material0;
	std::shared_ptr<SurfaceMaterial>   m_material1;
	std::shared_ptr<UnifiedColorImage> m_factor;

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

		TSdlUnifiedColorImage<OwnerType> factor("factor", &OwnerType::m_factor);
		factor.description("Factor that controls the contribution from each material.");
		factor.noDefault();
		factor.optional();// Some operation might not need a factor. Check factor at cook time.
		clazz.addField(factor);

		return clazz;
	}
};

}// end namespace ph
