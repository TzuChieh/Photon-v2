#include "Actor/Geometry/GInfiniteSphere.h"
#include "Core/Intersectable/PInfiniteSphere.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

namespace ph
{

GInfiniteSphere::GInfiniteSphere(real boundRadius) :
	Geometry(),
	m_boundRadius(boundRadius)
{}

void GInfiniteSphere::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	out_primitives.push_back(std::make_unique<PInfiniteSphere>(m_boundRadius, data.metadata));
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
	// TODO: should we expose this to SDL?
	PH_ASSERT_UNREACHABLE_SECTION();
	return std::make_unique<GInfiniteSphere>(0.0_r);
}

}// end namespace ph
	