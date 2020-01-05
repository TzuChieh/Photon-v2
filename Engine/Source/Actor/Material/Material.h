#pragma once

#include "Core/SurfaceBehavior/SurfaceBehavior.h"
#include "DataIO/SDL/TCommandInterface.h"

namespace ph
{

class InputPacket;
class CookingContext;
class PrimitiveMetadata;

class Material : public TCommandInterface<Material>
{
public:
	Material() = default;
	virtual ~Material() = default;

	virtual void genBehaviors(CookingContext& context, PrimitiveMetadata& metadata) const = 0;

// command interface
public:
	explicit Material(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

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
