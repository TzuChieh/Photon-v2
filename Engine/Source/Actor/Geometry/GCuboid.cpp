#include "Actor/Geometry/GCuboid.h"
#include "Core/Intersectable/PTriangle.h"
#include "Math/TVector3.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/PrimitiveBuildingMaterial.h"
#include "Common/assertion.h"
#include "FileIO/SDL/InputPacket.h"

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
	m_xLen(xLen), m_yLen(yLen), m_zLen(zLen), m_offset(offset)
{
	PH_ASSERT(xLen > 0.0_r && yLen > 0.0_r && zLen > 0.0_r);
}

GCuboid::GCuboid(const GCuboid& other) :
	m_xLen(other.m_xLen), 
	m_yLen(other.m_yLen), 
	m_zLen(other.m_zLen), 
	m_offset(other.m_offset)
{}

void GCuboid::genPrimitive(const PrimitiveBuildingMaterial& data,
                           std::vector<std::unique_ptr<Primitive>>& out_primitives) const
{
	if(!checkData(data, m_xLen, m_yLen, m_zLen))
	{
		return;
	}

	// TODO: UVW mapping

	const real halfXlen = m_xLen * 0.5_r;
	const real halfYlen = m_yLen * 0.5_r;
	const real halfZlen = m_zLen * 0.5_r;

	// 8 vertices of a cuboid
	const Vector3R vPPP = Vector3R( halfXlen,  halfYlen,  halfZlen).add(m_offset);
	const Vector3R vNPP = Vector3R(-halfXlen,  halfYlen,  halfZlen).add(m_offset);
	const Vector3R vNNP = Vector3R(-halfXlen, -halfYlen,  halfZlen).add(m_offset);
	const Vector3R vPNP = Vector3R( halfXlen, -halfYlen,  halfZlen).add(m_offset);
	const Vector3R vPPN = Vector3R( halfXlen,  halfYlen, -halfZlen).add(m_offset);
	const Vector3R vNPN = Vector3R(-halfXlen,  halfYlen, -halfZlen).add(m_offset);
	const Vector3R vNNN = Vector3R(-halfXlen, -halfYlen, -halfZlen).add(m_offset);
	const Vector3R vPNN = Vector3R( halfXlen, -halfYlen, -halfZlen).add(m_offset);

	// 12 triangles (all CCW)

	// xy-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPP, vNPP, vNNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPP, vNNP, vPNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPN, vPPN, vPNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPN, vPNN, vNNN));

	// yz-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vPPP, vPNP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vPNP, vPNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPP, vNPN, vNNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vNPP, vNNN, vNNP));

	// xz-plane
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vNPN, vNPP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPPN, vNPP, vPPP));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPNP, vNNP, vNNN));
	out_primitives.push_back(std::make_unique<PTriangle>(data.metadata, vPNP, vNNN, vPNN));
}

GCuboid& GCuboid::operator = (const GCuboid& rhs)
{
	m_xLen   = rhs.m_xLen;
	m_yLen   = rhs.m_yLen;
	m_zLen   = rhs.m_zLen;
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

// command interface

SdlTypeInfo GCuboid::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_GEOMETRY, "cuboid");
}

void GCuboid::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		const Vector3R minVertex = packet.getVector3("min-vertex", Vector3R(), DataTreatment::REQUIRED());
		const Vector3R maxVertex = packet.getVector3("max-vertex", Vector3R(), DataTreatment::REQUIRED());

		return std::make_unique<GCuboid>(minVertex, maxVertex);
	}));
}

}// end namespace ph