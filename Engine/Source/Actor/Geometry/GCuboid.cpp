#include "Actor/Geometry/GCuboid.h"
#include "Core/Intersectable/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "DataIO/SDL/InputPacket.h"
#include "Math/math.h"
#include "Actor/Geometry/GTriangleMesh.h"

#include <cmath>
#include <iostream>
#include <memory>

namespace ph
{

GCuboid::GCuboid() : 
	GCuboid(1)
{}

GCuboid::GCuboid(const real sideLength) : 
	GCuboid(sideLength, math::Vector3R(0, 0, 0))
{}

GCuboid::GCuboid(const real sideLength, const math::Vector3R& offset) :
	GCuboid(sideLength, sideLength, sideLength, offset)
{}

GCuboid::GCuboid(const real xLen, const real yLen, const real zLen) :
	GCuboid(xLen, yLen, zLen, math::Vector3R(0, 0, 0))
{}

GCuboid::GCuboid(const math::Vector3R& minVertex, const math::Vector3R& maxVertex) :
	GCuboid(maxVertex.x - minVertex.x,
	        maxVertex.y - minVertex.y,
	        maxVertex.z - minVertex.z,
	        (maxVertex + minVertex) * 0.5_r)
{}

GCuboid::GCuboid(const real xLen, const real yLen, const real zLen, const math::Vector3R& offset) :
	Geometry(),
	m_size(xLen, yLen, zLen), m_offset(offset), m_faceUVs(genNormalizedFaceUVs())
{
	PH_ASSERT(xLen > 0.0_r && yLen > 0.0_r && zLen > 0.0_r);
}

GCuboid::GCuboid(const GCuboid& other) :
	m_size(other.m_size),
	m_offset(other.m_offset),
	m_faceUVs(other.m_faceUVs)
{}

void GCuboid::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_size.x, m_size.y, m_size.z))
	{
		return;
	}

	GCuboid::genTriangulated()->genPrimitive(data, out_primitives);
}

std::shared_ptr<Geometry> GCuboid::genTriangulated() const
{
	auto triangleMesh = std::make_shared<GTriangleMesh>();

	const math::Vector3R halfSize = m_size * 0.5_r;

	// 8 vertices of a cuboid
	const auto vPPP = math::Vector3R( halfSize.x,  halfSize.y,  halfSize.z).add(m_offset);
	const auto vNPP = math::Vector3R(-halfSize.x,  halfSize.y,  halfSize.z).add(m_offset);
	const auto vNNP = math::Vector3R(-halfSize.x, -halfSize.y,  halfSize.z).add(m_offset);
	const auto vPNP = math::Vector3R( halfSize.x, -halfSize.y,  halfSize.z).add(m_offset);
	const auto vPPN = math::Vector3R( halfSize.x,  halfSize.y, -halfSize.z).add(m_offset);
	const auto vNPN = math::Vector3R(-halfSize.x,  halfSize.y, -halfSize.z).add(m_offset);
	const auto vNNN = math::Vector3R(-halfSize.x, -halfSize.y, -halfSize.z).add(m_offset);
	const auto vPNN = math::Vector3R( halfSize.x, -halfSize.y, -halfSize.z).add(m_offset);

	// 12 triangles (all CCW)

	// +z face (+y as upward)
	const int pz = math::constant::Z_AXIS;
	{
		GTriangle upperTriangle(vPPP, vNPP, vNNP);
		upperTriangle.setUVWa({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].minVertex.y, 0});

		GTriangle lowerTriangle(vPPP, vNNP, vPNP);
		lowerTriangle.setUVWa({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -z face (+y as upward)
	const int nz = math::constant::Z_AXIS + 3;
	{
		GTriangle upperTriangle(vNPN, vPPN, vPNN);
		upperTriangle.setUVWa({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].minVertex.y, 0});

		GTriangle lowerTriangle(vNPN, vPNN, vNNN);
		lowerTriangle.setUVWa({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +x face (+y as upward)
	const int px = math::constant::X_AXIS;
	{
		GTriangle upperTriangle(vPPN, vPPP, vPNP);
		upperTriangle.setUVWa({m_faceUVs[px].maxVertex.x, m_faceUVs[px].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[px].minVertex.x, m_faceUVs[px].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[px].minVertex.x, m_faceUVs[px].minVertex.y, 0});

		GTriangle lowerTriangle(vPPN, vPNP, vPNN);
		lowerTriangle.setUVWa({m_faceUVs[px].maxVertex.x, m_faceUVs[px].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[px].minVertex.x, m_faceUVs[px].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[px].maxVertex.x, m_faceUVs[px].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -x face (+y as upward)
	const int nx = math::constant::X_AXIS + 3;
	{
		GTriangle upperTriangle(vNPP, vNPN, vNNN);
		upperTriangle.setUVWa({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].minVertex.y, 0});

		GTriangle lowerTriangle(vNPP, vNNN, vNNP);
		lowerTriangle.setUVWa({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (-z as upward)
	const int py = math::constant::Y_AXIS;
	{
		GTriangle upperTriangle(vPPN, vNPN, vNPP);
		upperTriangle.setUVWa({m_faceUVs[py].maxVertex.x, m_faceUVs[py].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[py].minVertex.x, m_faceUVs[py].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[py].minVertex.x, m_faceUVs[py].minVertex.y, 0});

		GTriangle lowerTriangle(vPPN, vNPP, vPPP);
		lowerTriangle.setUVWa({m_faceUVs[py].maxVertex.x, m_faceUVs[py].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[py].minVertex.x, m_faceUVs[py].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[py].maxVertex.x, m_faceUVs[py].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (+z as upward)
	const int ny = math::constant::Y_AXIS + 3;
	{
		GTriangle upperTriangle(vPNP, vNNP, vNNN);
		upperTriangle.setUVWa({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		upperTriangle.setUVWb({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		upperTriangle.setUVWc({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].minVertex.y, 0});

		GTriangle lowerTriangle(vPNP, vNNN, vPNN);
		lowerTriangle.setUVWa({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		lowerTriangle.setUVWb({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].minVertex.y, 0});
		lowerTriangle.setUVWc({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].minVertex.y, 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	return triangleMesh;
}

GCuboid& GCuboid::operator = (const GCuboid& rhs)
{
	m_size   = rhs.m_size;
	m_offset = rhs.m_offset;

	return *this;
}

bool GCuboid::checkData(
	const PrimitiveBuildingMaterial& data, 
	const real xLen, const real yLen, const real zLen)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GCuboid::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(xLen <= 0.0_r || yLen <= 0.0_r || zLen <= 0.0_r)
	{
		std::cerr << "warning: at GCuboid::checkData(), GCuboid's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

auto GCuboid::genNormalizedFaceUVs()
	->std::array<math::AABB2D, 6>
{
	std::array<math::AABB2D, 6> faceUVs;
	faceUVs.fill({{0, 0}, {1, 1}});
	return faceUVs;
}

// command interface

SdlTypeInfo GCuboid::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "cuboid");
}

void GCuboid::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		const auto minVertex = packet.getVector3("min-vertex", math::Vector3R(0), DataTreatment::REQUIRED());
		const auto maxVertex = packet.getVector3("max-vertex", math::Vector3R(0), DataTreatment::REQUIRED());

		auto cuboid = std::make_unique<GCuboid>(minVertex, maxVertex);

		if(packet.hasQuaternion("px-face-uv"))
		{
			const auto uv = packet.getQuaternion("px-face-uv");
			cuboid->m_faceUVs[math::constant::X_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}
		
		if(packet.hasQuaternion("nx-face-uv"))
		{
			const auto uv = packet.getQuaternion("nx-face-uv");
			cuboid->m_faceUVs[math::constant::X_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("py-face-uv"))
		{
			const auto uv = packet.getQuaternion("py-face-uv");
			cuboid->m_faceUVs[math::constant::Y_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("ny-face-uv"))
		{
			const auto uv = packet.getQuaternion("ny-face-uv");
			cuboid->m_faceUVs[math::constant::Y_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("pz-face-uv"))
		{
			const auto uv = packet.getQuaternion("pz-face-uv");
			cuboid->m_faceUVs[math::constant::Z_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("nz-face-uv"))
		{
			const auto uv = packet.getQuaternion("nz-face-uv");
			cuboid->m_faceUVs[math::constant::Z_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		return cuboid;
	}));
}

}// end namespace ph
