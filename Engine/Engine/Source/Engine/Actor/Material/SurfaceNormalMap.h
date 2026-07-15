#pragma once

#include "Engine/Actor/Material/SurfaceMaterial.h"
#include "Engine/Actor/Material/Component/sdl_component_enums.h"
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
	ENormalMapFormat m_format;
	real m_strength;
	std::shared_ptr<Image> m_strengthMap;

public:
	PH_DEFINE_SDL_CLASS(SurfaceNormalMap, clazz)
	{
		clazz.typeName("surface-normal-map");
		clazz.docName("Surface Normal Map");
		clazz.description(
			"Normal mapping for a surface material. For paired value/map inputs, "
			"map inputs have higher precedence.");
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

		TSdlEnumField<OwnerType, ENormalMapFormat> format("format", &OwnerType::m_format);
		format.description("Format convention of the normal map.");
		format.optional();
		format.defaultTo(ENormalMapFormat::PXPYPZ_8Bits);
		clazz.addField(format);

		TSdlReal<OwnerType> strength("strength", &OwnerType::m_strength);
		strength.description(
			"Nonnegative scale applied to the tangent components of the normal map.");
		strength.optional();
		strength.defaultTo(1.0_r);
		clazz.addField(strength);

		TSdlReference<Image, OwnerType> strengthMap("strength-map", &OwnerType::m_strengthMap);
		strengthMap.description(
			"Texture-mapped strength.");
		strengthMap.optional();
		clazz.addField(strengthMap);
	}
};

}// end namespace ph
