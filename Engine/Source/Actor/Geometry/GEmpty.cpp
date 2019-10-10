#include "Actor/Geometry/GEmpty.h"
#include "Core/Intersectable/PEmpty.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

namespace ph
{

void GEmpty::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	out_primitives.push_back(std::make_unique<PEmpty>(data.metadata));
}

// command interface

SdlTypeInfo GEmpty::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "empty");
}

void GEmpty::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<GEmpty>();
	}));
}

}// end namespace ph
