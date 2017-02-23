#include "Actor/Geometry/GRectangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "FileIO/InputPacket.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle(const real width, const real height) :
	m_width(width), m_height(height)
{

}

GRectangle::GRectangle(const InputPacket& packet) : 
	Geometry(packet)
{
	const DataTreatment requiredData(EDataImportance::REQUIRED, "GRectangle needs width and height specified");
	m_width  = packet.getReal("width",  1.0_r, requiredData);
	m_height = packet.getReal("height", 1.0_r, requiredData);
}

GRectangle::~GRectangle() = default;

void GRectangle::discretize(const PrimitiveBuildingMaterial& data,
                            std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_width, m_height))
	{
		return;
	}

	const real halfWidth = m_width * 0.5_r;
	const real halfHeight = m_height * 0.5_r;

	const Vector3R vA(-halfWidth,  halfHeight, 0.0_r);// quadrant II
	const Vector3R vB(-halfWidth, -halfHeight, 0.0_r);// quadrant III
	const Vector3R vC( halfWidth, -halfHeight, 0.0_r);// quadrant IV
	const Vector3R vD( halfWidth,  halfHeight, 0.0_r);// quadrant I

	const Vector3R tA(0.0_r, 1.0_r, 0.0_r);// quadrant II
	const Vector3R tB(0.0_r, 0.0_r, 0.0_r);// quadrant III
	const Vector3R tC(1.0_r, 0.0_r, 0.0_r);// quadrant IV
	const Vector3R tD(1.0_r, 1.0_r, 0.0_r);// quadrant I

	PTriangle tri1(data.metadata, vA, vB, vD);
	tri1.setUVWa(tA);
	tri1.setUVWb(tB);
	tri1.setUVWc(tD);

	PTriangle tri2(data.metadata, vB, vC, vD);
	tri2.setUVWa(tB);
	tri2.setUVWb(tC);
	tri2.setUVWc(tD);

	// 2 triangles for a rectangle (both CCW)
	out_primitives.push_back(std::make_unique<PTriangle>(tri1));
	out_primitives.push_back(std::make_unique<PTriangle>(tri2));
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

}// end namespace ph