#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/SDL/TSdlResourceBase.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>
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
		const CookingContext& ctx,
		CookedMaterial& out_material) const = 0;

	/*! @brief Cook material using the provided context and output storage.
	*/
	void cook(const CookingContext& ctx, CookedMaterial& out_material) const;
	
	/*! @brief Priority for resolving overlapping volume optics.
	A value of 0 means the material does not contribute volume optics during actor cooking.
	*/
	virtual uint16 getOverlapPriority() const;

	void setInterfaceMask(std::shared_ptr<Image> interfaceMask);

private:
	std::shared_ptr<Image> m_interfaceMask;

public:
	PH_DEFINE_SDL_CLASS(Material, clazz)
	{
		clazz.typeName(std::string(sdl::category_to_string(CATEGORY)));
		clazz.docName("Material");
		clazz.description("Defines and models the appearance of scene elements.");
		clazz.addField(makeDisplayNameField<OwnerType>());

		TSdlReference<Image, OwnerType> interfaceMask("interface-mask", &OwnerType::m_interfaceMask);
		interfaceMask.description("Marks whether the material interface exists at each point.");
		interfaceMask.optional();
		clazz.addField(interfaceMask);
	}
};

inline uint16 Material::getOverlapPriority() const
{
	return 0;
}

}// end namespace ph
