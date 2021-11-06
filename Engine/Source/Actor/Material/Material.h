#pragma once

#include "DataIO/SDL/TSdlResourceBase.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "DataIO/SDL/sdl_interface.h"

namespace ph { class ActorCookingContext; }

namespace ph
{

class PrimitiveMetadata;

class Material : public TSdlResourceBase<ETypeCategory::REF_MATERIAL>
{
public:
	inline Material() = default;

	virtual void genBehaviors(ActorCookingContext& ctx, PrimitiveMetadata& metadata) const = 0;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<Material>)
	{
		ClassType clazz(sdl::category_to_string(CATEGORY));
		clazz.docName("Material");
		clazz.description("Defines and models the appearance of scene elements.");
		return clazz;
	}
};

}// end namespace ph
