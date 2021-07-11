#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "DataIO/SDL/ISdlResource.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

namespace ph { class ActorCookingContext; }

namespace ph
{

class PrimitiveMetadata;

class Material : public ISdlResource
{
public:
	static constexpr ETypeCategory CATEGORY = ETypeCategory::REF_MATERIAL;

public:
	inline Material() = default;

	virtual void genBehaviors(ActorCookingContext& ctx, PrimitiveMetadata& metadata) const = 0;

	ETypeCategory getCategory() const override; 

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Material>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
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
