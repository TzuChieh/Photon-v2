#include "Actor/Geometry/GCuboid.h"
#include "Core/Intersectable/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/math.h"

#include <cmath>
#include <iostream>
#include <memory>

namespace ph
{

GCuboid::GCuboid() : 
	GCuboid(1)
{}

GCuboid::GCuboid(const real sideLength) : 
	GCuboid(sideLength, Vector3R(0, 0, 0))
{}

GCuboid::GCuboid(const real sideLength, const Vector3R& offset) : 
	GCuboid(sideLength, sideLength, sideLength, offset)
{}

GCuboid::GCuboid(const real xLen, const real yLen, const real zLen) :
	GCuboid(xLen, yLen, zLen, Vector3R(0, 0, 0))
{}

GCuboid::GCuboid(const Vector3R& minVertex, const Vector3R& maxVertex) : 
	GCuboid(maxVertex.x - minVertex.x,
	        maxVertex.y - minVertex.y,
	        maxVertex.z - minVertex.z,
	        (maxVertex + minVertex) * 0.5_r)
{}

GCuboid::GCuboid(const real xLen, const real yLen, const real zLen, const Vector3R& offset) :
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

void GCuboid::genPrimitive(const PrimitiveBuildingMaterial& data,
                           std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_size.x, m_size.y, m_size.z))
	{
		return;
	}

	// TODO: UVW mapping

	const Vector3R halfSize = m_size * 0.5_r;

	// 8 vertices of a cuboid
	const Vector3R vPPP = Vector3R( halfSize.x,  halfSize.y,  halfSize.z).add(m_offset);
	const Vector3R vNPP = Vector3R(-halfSize.x,  halfSize.y,  halfSize.z).add(m_offset);
	const Vector3R vNNP = Vector3R(-halfSize.x, -halfSize.y,  halfSize.z).add(m_offset);
	const Vector3R vPNP = Vector3R( halfSize.x, -halfSize.y,  halfSize.z).add(m_offset);
	const Vector3R vPPN = Vector3R( halfSize.x,  halfSize.y, -halfSize.z).add(m_offset);
	const Vector3R vNPN = Vector3R(-halfSize.x,  halfSize.y, -halfSize.z).add(m_offset);
	const Vector3R vNNN = Vector3R(-halfSize.x, -halfSize.y, -halfSize.z).add(m_offset);
	const Vector3R vPNN = Vector3R( halfSize.x, -halfSize.y, -halfSize.z).add(m_offset);

	// 12 triangles (all CCW)

	// +z face (+y as upward)
	const int pz = math::Z_AXIS;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vPPP, vNPP, vNNP);
		upperTriangle->setUVWa({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vPPP, vNNP, vPNP);
		lowerTriangle->setUVWa({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[pz].minVertex.x, m_faceUVs[pz].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[pz].maxVertex.x, m_faceUVs[pz].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}

	// -z face (+y as upward)
	const int nz = math::Z_AXIS + 3;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vNPN, vPPN, vPNN);
		upperTriangle->setUVWa({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vNPN, vPNN, vNNN);
		lowerTriangle->setUVWa({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[nz].minVertex.x, m_faceUVs[nz].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[nz].maxVertex.x, m_faceUVs[nz].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}

	// +x face (+y as upward)
	const int px = math::X_AXIS;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vPPN, vPPP, vPNP);
		upperTriangle->setUVWa({m_faceUVs[px].maxVertex.x, m_faceUVs[px].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[px].minVertex.x, m_faceUVs[px].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[px].minVertex.x, m_faceUVs[px].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vPPN, vPNP, vPNN);
		lowerTriangle->setUVWa({m_faceUVs[px].maxVertex.x, m_faceUVs[px].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[px].minVertex.x, m_faceUVs[px].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[px].maxVertex.x, m_faceUVs[px].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}

	// -x face (+y as upward)
	const int nx = math::X_AXIS + 3;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vNPP, vNPN, vNNN);
		upperTriangle->setUVWa({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vNPP, vNNN, vNNP);
		lowerTriangle->setUVWa({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[nx].minVertex.x, m_faceUVs[nx].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[nx].maxVertex.x, m_faceUVs[nx].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}

	// +y face (-z as upward)
	const int py = math::Y_AXIS;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vPPN, vNPN, vNPP);
		upperTriangle->setUVWa({m_faceUVs[py].maxVertex.x, m_faceUVs[py].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[py].minVertex.x, m_faceUVs[py].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[py].minVertex.x, m_faceUVs[py].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vPPN, vNPP, vPPP);
		lowerTriangle->setUVWa({m_faceUVs[py].maxVertex.x, m_faceUVs[py].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[py].minVertex.x, m_faceUVs[py].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[py].maxVertex.x, m_faceUVs[py].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}

	// +y face (+z as upward)
	const int ny = math::Y_AXIS + 3;
	{
		auto upperTriangle = std::make_unique<PTriangle>(data.metadata, vPNP, vNNP, vNNN);
		upperTriangle->setUVWa({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		upperTriangle->setUVWb({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		upperTriangle->setUVWc({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].minVertex.y, 0});

		auto lowerTriangle = std::make_unique<PTriangle>(data.metadata, vPNP, vNNN, vPNN);
		lowerTriangle->setUVWa({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].maxVertex.y, 0});
		lowerTriangle->setUVWb({m_faceUVs[ny].minVertex.x, m_faceUVs[ny].minVertex.y, 0});
		lowerTriangle->setUVWc({m_faceUVs[ny].maxVertex.x, m_faceUVs[ny].minVertex.y, 0});

		out_primitives.push_back(std::move(upperTriangle));
		out_primitives.push_back(std::move(lowerTriangle));
	}
}

GCuboid& GCuboid::operator = (const GCuboid& rhs)
{
	m_size   = rhs.m_size;
	m_offset = rhs.m_offset;

	return *this;
}

bool GCuboid::checkData(const PrimitiveBuildingMaterial& data, const real xLen, const real yLen, const real zLen)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GCuboid::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(xLen <= 0.0f || yLen <= 0.0f || zLen <= 0.0f)
	{
		std::cerr << "warning: at GCuboid::checkData(), GCuboid's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

std::array<AABB2D, 6> GCuboid::genNormalizedFaceUVs()
{
	std::array<AABB2D, 6> faceUVs;
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
		const Vector3R minVertex = packet.getVector3("min-vertex", Vector3R(0), DataTreatment::REQUIRED());
		const Vector3R maxVertex = packet.getVector3("max-vertex", Vector3R(0), DataTreatment::REQUIRED());

		auto cuboid = std::make_unique<GCuboid>(minVertex, maxVertex);

		if(packet.hasQuaternion("px-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("px-face-uv");
			cuboid->m_faceUVs[math::X_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}
		
		if(packet.hasQuaternion("nx-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("nx-face-uv");
			cuboid->m_faceUVs[math::X_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("py-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("py-face-uv");
			cuboid->m_faceUVs[math::Y_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("ny-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("ny-face-uv");
			cuboid->m_faceUVs[math::Y_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("pz-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("pz-face-uv");
			cuboid->m_faceUVs[math::Z_AXIS] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		if(packet.hasQuaternion("nz-face-uv"))
		{
			const QuaternionR uv = packet.getQuaternion("nz-face-uv");
			cuboid->m_faceUVs[math::Z_AXIS + 3] = {{uv.x, uv.y}, {uv.z, uv.w}};
		}

		return cuboid;
	}));
}

}// end namespace ph