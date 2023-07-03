#pragma once

#include "Actor/Material/Material.h"
#include "SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class SurfaceOptics;
class SurfaceBehavior;

class SurfaceMaterial : public Material
{
public:
	SurfaceMaterial();

	virtual void genSurface(const CookingContext& ctx, SurfaceBehavior& behavior) const = 0;

	void genBehaviors(const CookingContext& ctx, PrimitiveMetadata& metadata) const override;

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
