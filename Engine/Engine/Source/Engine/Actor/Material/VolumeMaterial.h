#pragma once

#include "Engine/Actor/Material/Material.h"
#include "Engine/SDL/sdl_interface.h"

namespace ph
{

class VolumeMaterial : public Material
{
public:
	void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const override = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<VolumeMaterial>)
	{
		ClassType clazz("volume-material");
		clazz.docName("Volume Material");
		clazz.baseOn<Material>();
		return clazz;
	}
};

}// end namespace ph
