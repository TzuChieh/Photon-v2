#include "Actor/Geometry/GCuboid.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/PTriangle.h"
#include "Math/TVector3.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Math/math.h"
#include "Actor/Geometry/GTriangleMesh.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <cmath>
#include <iostream>
#include <memory>

namespace ph
{

void GCuboid::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	if(!checkData(m_size.x(), m_size.y(), m_size.z()))
	{
		return;
	}

	GCuboid::genTriangulated()->storeCooked(out_geometry, ctx);
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
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPP, vNPP, vNNP);
		upperTriangle.setUVWa({m_pzFaceUV.z(), m_pzFaceUV.w(), 0});
		upperTriangle.setUVWb({m_pzFaceUV.x(), m_pzFaceUV.w(), 0});
		upperTriangle.setUVWc({m_pzFaceUV.x(), m_pzFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPP, vNNP, vPNP);
		lowerTriangle.setUVWa({m_pzFaceUV.z(), m_pzFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_pzFaceUV.x(), m_pzFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_pzFaceUV.z(), m_pzFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -z face (+y as upward)
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vNPN, vPPN, vPNN);
		upperTriangle.setUVWa({m_nzFaceUV.z(), m_nzFaceUV.w(), 0});
		upperTriangle.setUVWb({m_nzFaceUV.x(), m_nzFaceUV.w(), 0});
		upperTriangle.setUVWc({m_nzFaceUV.x(), m_nzFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vNPN, vPNN, vNNN);
		lowerTriangle.setUVWa({m_nzFaceUV.z(), m_nzFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_nzFaceUV.x(), m_nzFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_nzFaceUV.z(), m_nzFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +x face (+y as upward)
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vPPP, vPNP);
		upperTriangle.setUVWa({m_pxFaceUV.z(), m_pxFaceUV.w(), 0});
		upperTriangle.setUVWb({m_pxFaceUV.x(), m_pxFaceUV.w(), 0});
		upperTriangle.setUVWc({m_pxFaceUV.x(), m_pxFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vPNP, vPNN);
		lowerTriangle.setUVWa({m_pxFaceUV.z(), m_pxFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_pxFaceUV.x(), m_pxFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_pxFaceUV.w(), m_pxFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// -x face (+y as upward)
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vNPP, vNPN, vNNN);
		upperTriangle.setUVWa({m_nxFaceUV.z(), m_nxFaceUV.w(), 0});
		upperTriangle.setUVWb({m_nxFaceUV.x(), m_nxFaceUV.w(), 0});
		upperTriangle.setUVWc({m_nxFaceUV.x(), m_nxFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vNPP, vNNN, vNNP);
		lowerTriangle.setUVWa({m_nxFaceUV.z(), m_nxFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_nxFaceUV.x(), m_nxFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_nxFaceUV.z(), m_nxFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (-z as upward)
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vNPN, vNPP);
		upperTriangle.setUVWa({m_pyFaceUV.z(), m_pyFaceUV.w(), 0});
		upperTriangle.setUVWb({m_pyFaceUV.x(), m_pyFaceUV.w(), 0});
		upperTriangle.setUVWc({m_pyFaceUV.x(), m_pyFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPPN, vNPP, vPPP);
		lowerTriangle.setUVWa({m_pyFaceUV.z(), m_pyFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_pyFaceUV.x(), m_pyFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_pyFaceUV.z(), m_pyFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	// +y face (+z as upward)
	{
		auto upperTriangle = TSdl<GTriangle>::make().setVertices(vPNP, vNNP, vNNN);
		upperTriangle.setUVWa({m_nyFaceUV.z(), m_nyFaceUV.w(), 0});
		upperTriangle.setUVWb({m_nyFaceUV.x(), m_nyFaceUV.w(), 0});
		upperTriangle.setUVWc({m_nyFaceUV.x(), m_nyFaceUV.y(), 0});

		auto lowerTriangle = TSdl<GTriangle>::make().setVertices(vPNP, vNNN, vPNN);
		lowerTriangle.setUVWa({m_nyFaceUV.z(), m_nyFaceUV.w(), 0});
		lowerTriangle.setUVWb({m_nyFaceUV.x(), m_nyFaceUV.y(), 0});
		lowerTriangle.setUVWc({m_nyFaceUV.z(), m_nyFaceUV.y(), 0});

		triangleMesh->addTriangle(upperTriangle);
		triangleMesh->addTriangle(lowerTriangle);
	}

	return triangleMesh;
}

GCuboid& GCuboid::setSize(const real sideLength)
{
	return setSize(sideLength, sideLength, sideLength);
}

GCuboid& GCuboid::setSize(const real xLen, const real yLen, const real zLen)
{
	return setSize(xLen, yLen, zLen, m_offset);
}

GCuboid& GCuboid::setSize(const math::Vector3R& minVertex, const math::Vector3R& maxVertex)
{
	return setSize(
		maxVertex.x() - minVertex.x(),
		maxVertex.y() - minVertex.y(),
		maxVertex.z() - minVertex.z(),
		(maxVertex + minVertex) * 0.5_r);
}

GCuboid& GCuboid::setSize(const real xLen, const real yLen, const real zLen, const math::Vector3R& offset)
{
	PH_ASSERT_GT(xLen, 0.0_r);
	PH_ASSERT_GT(yLen, 0.0_r);
	PH_ASSERT_GT(zLen, 0.0_r);

	m_size = {xLen, yLen, zLen};
	m_offset = offset;

	return *this;
}

bool GCuboid::checkData(const real xLen, const real yLen, const real zLen)
{
	if(xLen <= 0.0_r || yLen <= 0.0_r || zLen <= 0.0_r)
	{
		PH_DEFAULT_LOG(Error,
			"at GCuboid::checkData(), GCuboid's dimensions (x: {}, y: {}, z: {}) are zero or negative",
			xLen, yLen, zLen);

		return false;
	}

	return true;
}

bool GCuboid::checkData(
	const PrimitiveBuildingMaterial& data, 
	const real xLen, 
	const real yLen, 
	const real zLen)
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

math::Vector4R GCuboid::makeNormalizedFaceUV()
{
	return {0, 0, 1, 1};
}

}// end namespace ph
