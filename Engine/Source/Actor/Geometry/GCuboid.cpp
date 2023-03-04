#include "Actor/Geometry/GCuboid.h"
#include "Core/Intersectable/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "Math/math.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Common/logging.h"

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
	GCuboid(maxVertex.x() - minVertex.x(),
	        maxVertex.y() - minVertex.y(),
	        maxVertex.z() - minVertex.z(),
	        (maxVertex + minVertex) * 0.5_r)
{}

GCuboid::GCuboid(const real xLen, const real yLen, const real zLen, const math::Vector3R& offset) :
	Geometry(),
	m_size(xLen, yLen, zLen), m_offset(offset), m_faceUVs(genNormalizedFaceUVs())
{
	PH_ASSERT(xLen > 0.0_r && yLen > 0.0_r && zLen > 0.0_r);
}

void GCuboid::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx,
	const GeometryCookConfig& config) const
{
	if(!checkData(m_size.x(), m_size.y(), m_size.z()))
	{
		return;
	}

	GCuboid::genTriangulated()->storeCooked(out_geometry, ctx, config);
}

void GCuboid::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_size.x(), m_size.y(), m_size.z()))
	{
		return;
	}

	GCuboid::genTriangulated()->genPrimitive(data, out_primitives);
}

std::shared_ptr<Geometry> GCuboid::genTriangulated() const
{
	auto triangleMesh = TSdl<GTriangleMesh>::makeResource();

	const math::Vector3R halfSize = m_size * 0.5_r;

	// 8 vertices of a cuboid
	const auto vPPP = math::Vector3R( halfSize.x(),  halfSize.y(),  halfSize.z()).add(m_offset);
	const auto vNPP = math::Vector3R(-halfSize.x(),  halfSize.y(),  halfSize.z()).add(m_offset);
	const auto vNNP = math::Vector3R(-halfSize.x(), -halfSize.y(),  halfSize.z()).add(m_offset);
	const auto vPNP = math::Vector3R( halfSize.x(), -halfSize.y(),  halfSize.z()).add(m_offset);
	const auto vPPN = math::Vector3R( halfSize.x(),  halfSize.y(), -halfSize.z()).add(m_offset);
	const auto vNPN = math::Vector3R(-halfSize.x(),  halfSize.y(), -halfSize.z()).add(m_offset);
	const auto vNNN = math::Vector3R(-halfSize.x(), -halfSize.y(), -halfSize.z()).add(m_offset);
	const auto vPNN = math::Vector3R( halfSize.x(), -halfSize.y(), -halfSize.z()).add(m_offset);

	// 12 triangles (all CCW)

	// +z face (+y as upward)
	const int pz = math::constant::Z_AXIS;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPP, vNPP, vNNP);
		upperTriangle.setUVWa({m_faceUVs[pz].getMaxVertex().x(), m_faceUVs[pz].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[pz].getMinVertex().x(), m_faceUVs[pz].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[pz].getMinVertex().x(), m_faceUVs[pz].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPP, vNNP, vPNP);
		lowerTriangle.setUVWa({m_faceUVs[pz].getMaxVertex().x(), m_faceUVs[pz].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[pz].getMinVertex().x(), m_faceUVs[pz].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[pz].getMaxVertex().x(), m_faceUVs[pz].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -z face (+y as upward)
	const int nz = math::constant::Z_AXIS + 3;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vNPN, vPPN, vPNN);
		upperTriangle.setUVWa({m_faceUVs[nz].getMaxVertex().x(), m_faceUVs[nz].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[nz].getMinVertex().x(), m_faceUVs[nz].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[nz].getMinVertex().x(), m_faceUVs[nz].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vNPN, vPNN, vNNN);
		lowerTriangle.setUVWa({m_faceUVs[nz].getMaxVertex().x(), m_faceUVs[nz].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[nz].getMinVertex().x(), m_faceUVs[nz].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[nz].getMaxVertex().x(), m_faceUVs[nz].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +x face (+y as upward)
	const int px = math::constant::X_AXIS;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vPPP, vPNP);
		upperTriangle.setUVWa({m_faceUVs[px].getMaxVertex().x(), m_faceUVs[px].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[px].getMinVertex().x(), m_faceUVs[px].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[px].getMinVertex().x(), m_faceUVs[px].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vPNP, vPNN);
		lowerTriangle.setUVWa({m_faceUVs[px].getMaxVertex().x(), m_faceUVs[px].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[px].getMinVertex().x(), m_faceUVs[px].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[px].getMaxVertex().x(), m_faceUVs[px].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -x face (+y as upward)
	const int nx = math::constant::X_AXIS + 3;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vNPP, vNPN, vNNN);
		upperTriangle.setUVWa({m_faceUVs[nx].getMaxVertex().x(), m_faceUVs[nx].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[nx].getMinVertex().x(), m_faceUVs[nx].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[nx].getMinVertex().x(), m_faceUVs[nx].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vNPP, vNNN, vNNP);
		lowerTriangle.setUVWa({m_faceUVs[nx].getMaxVertex().x(), m_faceUVs[nx].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[nx].getMinVertex().x(), m_faceUVs[nx].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[nx].getMaxVertex().x(), m_faceUVs[nx].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (-z as upward)
	const int py = math::constant::Y_AXIS;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vNPN, vNPP);
		upperTriangle.setUVWa({m_faceUVs[py].getMaxVertex().x(), m_faceUVs[py].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[py].getMinVertex().x(), m_faceUVs[py].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[py].getMinVertex().x(), m_faceUVs[py].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vNPP, vPPP);
		lowerTriangle.setUVWa({m_faceUVs[py].getMaxVertex().x(), m_faceUVs[py].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[py].getMinVertex().x(), m_faceUVs[py].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[py].getMaxVertex().x(), m_faceUVs[py].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (+z as upward)
	const int ny = math::constant::Y_AXIS + 3;
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPNP, vNNP, vNNN);
		upperTriangle.setUVWa({m_faceUVs[ny].getMaxVertex().x(), m_faceUVs[ny].getMaxVertex().y(), 0});
		upperTriangle.setUVWb({m_faceUVs[ny].getMinVertex().x(), m_faceUVs[ny].getMaxVertex().y(), 0});
		upperTriangle.setUVWc({m_faceUVs[ny].getMinVertex().x(), m_faceUVs[ny].getMinVertex().y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPNP, vNNN, vPNN);
		lowerTriangle.setUVWa({m_faceUVs[ny].getMaxVertex().x(), m_faceUVs[ny].getMaxVertex().y(), 0});
		lowerTriangle.setUVWb({m_faceUVs[ny].getMinVertex().x(), m_faceUVs[ny].getMinVertex().y(), 0});
		lowerTriangle.setUVWc({m_faceUVs[ny].getMaxVertex().x(), m_faceUVs[ny].getMinVertex().y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	return triangleMesh;
}

bool GCuboid::checkData(const real xLen, const real yLen, const real zLen)
{
	if(xLen <= 0.0_r || yLen <= 0.0_r || zLen <= 0.0_r)
	{
		PH_DEFAULT_LOG_ERROR(
			"at GCuboid::checkData(), GCuboid's dimensions (x: {}, y: {}, z: {}) are zero or negative",
			xLen, yLen, zLen);

		return false;
	}

	return true;
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

}// end namespace ph
