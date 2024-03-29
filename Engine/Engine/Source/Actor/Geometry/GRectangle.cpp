#include "Actor/Geometry/GRectangle.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/PTriangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GTriangle.h"
#include "Math/TVector3.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <Common/logging.h>

#include <iostream>

namespace ph
{

void GRectangle::storeCooked(
	CookedGeometry& out_geometry,
	const CookingContext& ctx) const
{
	if(!checkData(m_width, m_height))
	{
		return;
	}

	genTriangleMesh()->storeCooked(out_geometry, ctx);
}

void GRectangle::genPrimitive(
	const PrimitiveBuildingMaterial& data,
	std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_width, m_height))
	{
		return;
	}

	genTriangleMesh()->genPrimitive(data, out_primitives);
}

std::shared_ptr<Geometry> GRectangle::genTransformed(
	const math::StaticAffineTransform& transform) const
{
	return genTriangleMesh()->genTransformed(transform);
}

GRectangle& GRectangle::setWidth(const real width)
{
	m_width = width;

	return *this;
}

GRectangle& GRectangle::setHeight(const real height)
{
	m_height = height;

	return *this;
}

GRectangle& GRectangle::setTexCoordScale(const real scale)
{
	m_texCoordScale = scale;

	return *this;
}

std::shared_ptr<GTriangleMesh> GRectangle::genTriangleMesh() const
{
	// TODO: check data

	const real halfWidth  = m_width * 0.5_r;
	const real halfHeight = m_height * 0.5_r;

	const math::Vector3R vA(-halfWidth,  halfHeight, 0.0_r);// quadrant II
	const math::Vector3R vB(-halfWidth, -halfHeight, 0.0_r);// quadrant III
	const math::Vector3R vC( halfWidth, -halfHeight, 0.0_r);// quadrant IV
	const math::Vector3R vD( halfWidth,  halfHeight, 0.0_r);// quadrant I

	const math::Vector3R tA(0.0_r, 1.0_r, 0.0_r);// quadrant II
	const math::Vector3R tB(0.0_r, 0.0_r, 0.0_r);// quadrant III
	const math::Vector3R tC(1.0_r, 0.0_r, 0.0_r);// quadrant IV
	const math::Vector3R tD(1.0_r, 1.0_r, 0.0_r);// quadrant I

	// 2 triangles for a rectangle (both CCW)

	auto tri1 = TSdl<GTriangle>::make().setVertices(vA, vB, vD);
	tri1.setUVWa(tA.mul(m_texCoordScale));
	tri1.setUVWb(tB.mul(m_texCoordScale));
	tri1.setUVWc(tD.mul(m_texCoordScale));

	auto tri2 = TSdl<GTriangle>::make().setVertices(vB, vC, vD);
	tri2.setUVWa(tB.mul(m_texCoordScale));
	tri2.setUVWb(tC.mul(m_texCoordScale));
	tri2.setUVWc(tD.mul(m_texCoordScale));

	auto triMeshRes = TSdl<GTriangleMesh>::makeResource();
	triMeshRes->addTriangle(tri1);
	triMeshRes->addTriangle(tri2);

	return triMeshRes;
}

bool GRectangle::checkData(const PrimitiveBuildingMaterial& data, const real width, const real height)
{
	if(!data.metadata)
	{
		std::cerr << "warning: at GRectangle::checkData(), no PrimitiveMetadata" << std::endl;
		return false;
	}

	if(width <= 0.0_r || height <= 0.0_r)
	{
		std::cerr << "warning: at GRectangle::checkData(), GRectangle's dimension is zero or negative" << std::endl;
		return false;
	}

	return true;
}

bool GRectangle::checkData(const real width, const real height)
{
	if(width <= 0.0_r || height <= 0.0_r)
	{
		PH_DEFAULT_LOG(Error,
			"at GRectangle::checkData(), rectangle dimensions ({}, {}) are zero or negative",
			width, height);

		return false;
	}

	return true;
}

}// end namespace ph
