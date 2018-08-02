#include "Actor/Geometry/GInfiniteSphere.h"
#include "Core/Intersectable/PInfiniteSphere.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

namespace ph
{

GInfiniteSphere::GInfiniteSphere() : 
	Geometry()
{}

void GInfiniteSphere::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	out_primitives.push_back(std::make_unique<PInfiniteSphere>(data.metadata));
}

// command interface

SdlTypeInfo GInfiniteSphere::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "infinite-sphere");
}

void GInfiniteSphere::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader(ciLoad));
}

std::unique_ptr<GInfiniteSphere> GInfiniteSphere::ciLoad(const InputPacket& packet)
{
	return std::make_unique<GInfiniteSphere>();
}

}// end namespace ph
	