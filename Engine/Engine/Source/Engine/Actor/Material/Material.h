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

	/*! @brief Create a `CookedMaterial` that contains data suitable for rendering.
	*/
	CookedMaterial* createCooked(const CookingContext& ctx) const;

	uint16 getOverlapPriority() const;

private:
	uint16 m_overlapPriority;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<Material>)
	{
		ClassType clazz(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Material");
		clazz.description("Defines and models the appearance of scene elements.");

		TSdlUInt16<OwnerType> priority("overlap-priority", &OwnerType::m_overlapPriority);
		priority.description(
			"Determines who takes precedence when multiple materials are overlapping. "
			"Higher value has higher priority and 0 is the lowest priority. "
			"The priority defaults to 0. When applied to mediums, overlapping materials with "
			"equal priorities results in undefined behavior.");
		priority.defaultTo(0);
		priority.optional();
		clazz.addField(priority);

		return clazz;
	}
};

inline uint16 Material::getOverlapPriority() const
{
	return m_overlapPriority;
}

}// end namespace ph
