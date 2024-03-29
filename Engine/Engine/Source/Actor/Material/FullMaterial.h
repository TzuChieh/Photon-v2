#pragma once

#include "Actor/Material/Material.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class SurfaceMaterial;
class VolumeMaterial;

// TODO: volume material

class FullMaterial final : public Material
{
public:
	FullMaterial();
	explicit FullMaterial(const std::shared_ptr<SurfaceMaterial>& surfaceMaterial);

	void genBehaviors(const CookingContext& ctx, PrimitiveMetadata& metadata) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_surfaceMaterial;
	std::shared_ptr<VolumeMaterial>  m_interiorMaterial;
	std::shared_ptr<VolumeMaterial>  m_exteriorMaterial;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<FullMaterial>)
	{
		ClassType clazz("full");
		clazz.docName("Full Material");
		clazz.description("A material model that combines surface and volume properties.");
		clazz.baseOn<Material>();

		TSdlReference<SurfaceMaterial, OwnerType> surfaceMaterial("surface", &OwnerType::m_surfaceMaterial);
		surfaceMaterial.description("A surface material.");
		surfaceMaterial.optional();
		clazz.addField(surfaceMaterial);

		TSdlReference<VolumeMaterial, OwnerType> interiorMaterial("interior", &OwnerType::m_interiorMaterial);
		interiorMaterial.description("A volume material describing the inside of the surface.");
		interiorMaterial.optional();
		clazz.addField(interiorMaterial);

		TSdlReference<VolumeMaterial, OwnerType> exteriorMaterial("exterior", &OwnerType::m_exteriorMaterial);
		exteriorMaterial.description("A volume material describing the outside of the surface.");
		exteriorMaterial.optional();
		clazz.addField(exteriorMaterial);

		return clazz;
	}
};

}// end namespace ph
