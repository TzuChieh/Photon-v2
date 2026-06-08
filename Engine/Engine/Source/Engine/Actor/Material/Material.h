#pragma once

#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/SDL/sdl_interface.h"

#include <string>

namespace ph { class CookingContext; }
namespace ph { class CookedMaterial; }

namespace ph
{

class Material : public TSdlResourceBase<ESdlTypeCategory::Ref_Material>
{
public:
	/*! @brief Store data suitable for rendering into `out_material`.
	*/
	virtual void storeCooked(
		CookedMaterial& out_material,
		const CookingContext& ctx) const = 0;

	/*! @brief Create a `CookedMaterial` that contains data suitable for rendering.
	*/
	CookedMaterial* createCooked(const CookingContext& ctx) const;

	/*! @brief Priority for resolving overlapping volume optics.
	A value of 0 means the material does not contribute volume optics during actor cooking.
	*/
	virtual uint16 getOverlapPriority() const;

public:
	PH_DEFINE_SDL_CLASS(Material, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Material");
		clazz.description("Defines and models the appearance of scene elements.");
	}
};

inline uint16 Material::getOverlapPriority() const
{
	return 0;
}

}// end namespace ph
