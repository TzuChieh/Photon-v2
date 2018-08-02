#pragma once

#include "Actor/Geometry/Geometry.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class GInfiniteSphere final : public Geometry, public TCommandInterface<GInfiniteSphere>
{
public:
	GInfiniteSphere();

	void genPrimitive(
		const PrimitiveBuildingMaterial&         data,
		std::vector<std::unique_ptr<Primitive>>& out_primitives) const override;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<GInfiniteSphere> ciLoad(const InputPacket& packet);
};

}// end namespace ph