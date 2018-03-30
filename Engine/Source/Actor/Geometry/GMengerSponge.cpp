#include "Actor/Geometry/GMengerSponge.h"
#include "FileIO/InputPacket.h"
#include "Common/assertion.h"
#include "Actor/Geometry/GCuboid.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>
#include <vector>
#include <cmath>

namespace ph
{

GMengerSponge::GMengerSponge() : 
	GMengerSponge(3)
{}

GMengerSponge::GMengerSponge(const uint32 numIteration) :
	Geometry(), 
	m_numIteration(numIteration)
{}

GMengerSponge::~GMengerSponge() = default;

void GMengerSponge::genPrimitive(
	const PrimitiveBuildingMaterial&         data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(data.metadata == nullptr)
	{
		std::cerr << "warning: at GMengerSponge::genPrimitive(), "
		          << "requires metadata" << std::endl;
		return;
	}

	std::vector<GCuboid> cubes;
	genMengerSpongeRecursive(
		Vector3R(-0.5_r, -0.5_r, -0.5_r),
		Vector3R( 0.5_r,  0.5_r,  0.5_r),
		0,
		cubes);
	for(const auto& cube : cubes)
	{
		cube.genPrimitive(data, out_primitives);
	}
}

// Reference: http://woo4.me/wootracer/menger-sponge/
//
void GMengerSponge::genMengerSpongeRecursive(
	const Vector3R&       minVertex,
	const Vector3R&       maxVertex,
	const uint32          currentIteration,
	std::vector<GCuboid>& cubes) const
{
	PH_ASSERT(currentIteration <= m_numIteration);
	if(currentIteration == m_numIteration)
	{
		cubes.push_back(GCuboid(minVertex, maxVertex));
		return;
	}

	const Vector3R currentExtends = maxVertex.sub(minVertex);
	const Vector3R nextExtends    = currentExtends / 3.0_r;
	for(int iz = 0; iz < 3; iz++)
	{
		for(int iy = 0; iy < 3; iy++)
		{
			for(int ix = 0; ix < 3; ix++)
			{
				const bool isHollow = ((iz == 1 ? 1 : 0) + 
				                       (iy == 1 ? 1 : 0) + 
				                       (ix == 1 ? 1 : 0)) > 1;
				if(!isHollow)
				{
					const Vector3R nextMinVertex(
						minVertex.x + nextExtends.x * ix,
						minVertex.y + nextExtends.y * iy,
						minVertex.z + nextExtends.z * iz);
					const Vector3R nextMaxVertex(
						minVertex.x + nextExtends.x * (ix + 1),
						minVertex.y + nextExtends.y * (iy + 1),
						minVertex.z + nextExtends.z * (iz + 1));
					genMengerSpongeRecursive(
						nextMinVertex,
						nextMaxVertex,
						currentIteration + 1,
						cubes);
				}
			}// end ix
		}// end iy
	}// end iz
}

std::shared_ptr<Geometry> GMengerSponge::genTransformApplied(const StaticTransform& transform) const
{
	return nullptr;
}

// command interface

SdlTypeInfo GMengerSponge::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "menger-sponge");
}

void GMengerSponge::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		integer numIteration = packet.getInteger("iterations", 3);
		if(numIteration < 0)
		{
			std::cerr << "warning: menger sponge with negative number of iteration, "
			          << "will use 3 instead" << std::endl;
			numIteration = 3;
		}
		return std::make_unique<GMengerSponge>(static_cast<uint32>(numIteration));
	}));
}

}// end namespace ph