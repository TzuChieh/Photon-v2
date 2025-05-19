#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/Core/SurfaceBehavior/SurfaceBehavior.h"
#include "Engine/SDL/sdl_interface.h"

#include <string>

namespace ph { class CookingContext; }

namespace ph
{

class PrimitiveMetadata;
class CookedMaterial;
class CookingContext;

class Material : public TSdlResourceBase<ESdlTypeCategory::Ref_Material>
{
public:
	/*! @brief Store data suitable for rendering into `out_material`.
	*/
	virtual void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const = 0;

	virtual void genBehaviors(const CookingContext& ctx, PrimitiveMetadata& metadata) const = 0;

	/*! @brief Create a `CookedMaterial` that contains data suitable for rendering.
	*/
	CookedMaterial* createCooked(const CookingContext& ctx) const;

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
