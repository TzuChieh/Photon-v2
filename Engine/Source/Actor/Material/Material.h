#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Actor/SDLExtension/sdl_interface_extended.h"

namespace ph
{

class InputPacket;
class CookingContext;
class PrimitiveMetadata;

class Material : public TCommandInterface<Material>
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
		clazz.setDescription("Defines and models the appearance of scene elements.");
		return clazz;
	}

	explicit Material(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline ETypeCategory Material::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  material </category>
	<type_name> material </type_name>

	<name> Material </name>
	<description>
		Defines and models the appearance of scene elements.
	</description>

	</SDL_interface>
*/
