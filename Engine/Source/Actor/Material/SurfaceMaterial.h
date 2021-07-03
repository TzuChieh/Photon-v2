#pragma once

#include "Actor/Material/Material.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

#include <memory>

namespace ph
{

class SurfaceOptics;
class SurfaceBehavior;

class SurfaceMaterial : public Material
{
public:
	SurfaceMaterial();

	virtual void genSurface(CookingContext& context, SurfaceBehavior& behavior) const = 0;

	void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SurfaceMaterial>)
	{
		ClassType clazz("surface-material");
		clazz.setBase<Material>();
		return clazz;
	}
};

}// end namespace ph
