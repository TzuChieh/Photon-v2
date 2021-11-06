#pragma once

#include "Actor/Material/Material.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

class SurfaceOptics;
class SurfaceBehavior;

class SurfaceMaterial : public Material
{
public:
	SurfaceMaterial();

	virtual void genSurface(ActorCookingContext& ctx, SurfaceBehavior& behavior) const = 0;

	void genBehaviors(ActorCookingContext& ctx, PrimitiveMetadata& metadata) const override;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<SurfaceMaterial>)
	{
		ClassType clazz("surface-material");
		clazz.docName("Surface Material");
		clazz.baseOn<Material>();
		return clazz;
	}
};

}// end namespace ph
