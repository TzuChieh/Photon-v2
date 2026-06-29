#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Image/Image.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class SurfaceNormalMap : public SurfaceMaterial
{
public:
	void storeCooked(
		const CookingContext& ctx,
		CookedMaterial& out_material) const override;

private:
	std::shared_ptr<SurfaceMaterial> m_material;
	std::shared_ptr<Image> m_map;

public:
	PH_DEFINE_SDL_CLASS(SurfaceNormalMap, clazz)
	{
		clazz.typeName("surface-normal-map");
		clazz.docName("Surface Normal Map");
		clazz.description("Normal mapping for a surface material.");
		clazz.baseOn<SurfaceMaterial>();

		TSdlReference<SurfaceMaterial, OwnerType> material("material", &OwnerType::m_material);
		material.description("The material which normal is going to be perturbed.");
		material.required();
		clazz.addField(material);

		TSdlReference<Image, OwnerType> map("map", &OwnerType::m_map);
		map.description(
			"A map that records the perturbed orientation of surface normal.");
		map.required();
		clazz.addField(map);
	}
};

}// end namespace ph
