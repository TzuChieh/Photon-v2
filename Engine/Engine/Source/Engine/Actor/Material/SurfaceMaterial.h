#pragma once

#include "Engine/Actor/Material/Material.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class SurfaceOptics;
class SurfaceBehavior;

class SurfaceMaterial : public Material
{
public:
	void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const override = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<SurfaceMaterial>)
	{
		ClassType clazz("surface-material");
		clazz.docName("Surface Material");
		clazz.baseOn<Material>();
		return clazz;
	}
};

}// end namespace ph
