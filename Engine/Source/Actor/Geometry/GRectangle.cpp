#include "Actor/Geometry/GRectangle.h"
#include "Core/Intersectable/PTriangle.h"
#include "Actor/Geometry/GTriangleMesh.h"
#include "Actor/Geometry/GTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "FileIO/SDL/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle() : 
	GRectangle(1, 1)
{}

GRectangle::GRectangle(const real width, const real height) :
	m_width(width), m_height(height),
	m_texCoordScale(1)
{}

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
	const StaticAffineTransform& transform) const
{
	return genTriangleMesh()->genTransformed(transform);
}

void GRectangle::setTexCoordScale(const real scale)
{
	m_texCoordScale = scale;
}

std::shared_ptr<GTriangleMesh> GRectangle::genTriangleMesh() const
{
	// TODO: check data

	const real halfWidth  = m_width * 0.5_r;
	const real halfHeight = m_height * 0.5_r;

	const Vector3R vA(-halfWidth,  halfHeight, 0.0_r);// quadrant II
	const Vector3R vB(-halfWidth, -halfHeight, 0.0_r);// quadrant III
	const Vector3R vC( halfWidth, -halfHeight, 0.0_r);// quadrant IV
	const Vector3R vD( halfWidth,  halfHeight, 0.0_r);// quadrant I

	const Vector3R tA(0.0_r, 1.0_r, 0.0_r);// quadrant II
	const Vector3R tB(0.0_r, 0.0_r, 0.0_r);// quadrant III
	const Vector3R tC(1.0_r, 0.0_r, 0.0_r);// quadrant IV
	const Vector3R tD(1.0_r, 1.0_r, 0.0_r);// quadrant I

	// 2 triangles for a rectangle (both CCW)

	GTriangle tri1(vA, vB, vD);
	tri1.setUVWa(tA.mul(m_texCoordScale));
	tri1.setUVWb(tB.mul(m_texCoordScale));
	tri1.setUVWc(tD.mul(m_texCoordScale));

	GTriangle tri2(vB, vC, vD);
	tri2.setUVWa(tB.mul(m_texCoordScale));
	tri2.setUVWb(tC.mul(m_texCoordScale));
	tri2.setUVWc(tD.mul(m_texCoordScale));

	auto triMesh = std::make_shared<GTriangleMesh>();
	triMesh->addTriangle(tri1);
	triMesh->addTriangle(tri2);

	return triMesh;
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

// command interface

SdlTypeInfo GRectangle::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "rectangle");
}

void GRectangle::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<GRectangle>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<GRectangle> GRectangle::ciLoad(const InputPacket& packet)
{
	const DataTreatment requiredData(EDataImportance::REQUIRED, 
	                                 "GRectangle needs width and height specified");

	const real width  = packet.getReal("width",  1.0_r, requiredData);
	const real height = packet.getReal("height", 1.0_r, requiredData);

	auto rectangle = std::make_unique<GRectangle>(width, height);
	rectangle->setTexCoordScale(packet.getReal("texcoord-scale", 1.0_r));
	return rectangle;
}

}// end namespace ph