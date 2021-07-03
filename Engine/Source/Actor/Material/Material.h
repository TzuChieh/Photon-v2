#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "DataIO/SDL/ISdlResource.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

namespace ph
{

class CookingContext;
class PrimitiveMetadata;

class Material : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_MATERIAL;

public:
	inline Material() = default;

	virtual void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const = 0;

	ETypeCategory getCategory() const override; 

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Material>)
	{
		ClassType clazz("material");
		clazz.description("Defines and models the appearance of scene elements.");
		return clazz;
	}
};

// In-header Implementations:

inline ETypeCategory Material::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
