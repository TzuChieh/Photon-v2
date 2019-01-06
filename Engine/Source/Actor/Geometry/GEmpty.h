#pragma once

#include "Actor/Geometry/Geometry.h"

#include <vector>
#include <memory>

namespace ph
{

class GEmpty : public Geometry, public TCommandInterface<GEmpty>
{
public:
	GEmpty() = default;

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data, 
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  geometry          </category>
	<type_name> empty             </type_name>
	<extend>    geometry.geometry </extend>

	<name> Empty </name>
	<description>
		It is just an empty geometry. Nothing is there.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/