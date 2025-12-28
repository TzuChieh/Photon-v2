#pragma once

#include "Engine/Actor/Material/Material.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class SurfaceMaterial : public Material
{
public:
	void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const override = 0;

public:
	PH_DEFINE_SDL_CLASS(SurfaceMaterial, clazz)
	{
		clazz.typeName("surface-material");
		clazz.docName("Surface Material");
		clazz.baseOn<Material>();
	}
};

}// end namespace ph
