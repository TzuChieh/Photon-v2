#pragma once

#include "SDL/TSdlResourceBase.h"
#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "SDL/sdl_interface.h"

#include <string>

namespace ph { class CookingContext; }

namespace ph
{

class PrimitiveMetadata;

class Material : public TSdlResourceBase<ESdlTypeCategory::Ref_Material>
{
public:
	inline Material() = default;

	virtual void genBehaviors(CookingContext& ctx, PrimitiveMetadata& metadata) const = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Material>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Material");
		clazz.description("Defines and models the appearance of scene elements.");
		return clazz;
	}
};

}// end namespace ph
